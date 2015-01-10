#include "forward.h"

#include "lodepng.h"

#define BG_R		58
#define BG_G		57
#define BG_B		65
#define CANDIDATE_R	255
#define CANDIDATE_G	255
#define CANDIDATE_B	255
#define CORRECT_R	154
#define CORRECT_G	221
#define CORRECT_B	114
#define	INCORRECT_R	236
#define	INCORRECT_G	81
#define	INCORRECT_B	81
#define SELECTED_R	103
#define SELECTED_G	168
#define SELECTED_B	196
#define RESULT_R	219
#define RESULT_G	79
#define RESULT_B	79
#define TOLERANCE	25
#define SCAN_POS	0.1
#define SCAN_START	0.3
#define SCAN_IMG	0.5
#define SCAN_END	0.9
#define AD_DISMISS	0.3
#define AD_R		0
#define AD_G		0
#define AD_B		0
#define MATCH_NEXT	0.72
#define WAITING_R	233
#define WAITING_G	103
#define WAITING_B	121


MMOperate::MMOperate(int success_rate): dump(this), accuracy(success_rate), count(0)
{
	srand(time(NULL));
}

void MMOperate::start()
{
	// Main Cycle
	dump.listen();
}

void MMOperate::gotAnswer(string ans, vector<bool> is_image)
{
	count+=10;
	if (count >100)
	{
		count = 0;
	}
	if (count > accuracy)
	{
		// Fake answers
		for (int i = 0; i < 7; ++i)
		{
			ans.push_back('1'+rand()%4);
		}
		cout << "Outputting random Answers" << endl;
	} else
	{
		cout << "Started on: " << ans << endl;
	}
	

	// MMDump callback "protocol"

	// Phase 1. Got the answers, begin selecting
	for (int i = 0; i < ans.size(); ++i)
	{
		// Loop for each question
		while(!selectAnswer(ans[i]-'1', is_image[i])){
			system("sleep 0.2");
		}
	}

	// Phase 2. Proceed to the result screen
	while(!dismissAd())
	{
		system("sleep 0.5");
	}
	cout << "Starting new session" << endl;

	while(!startSession())
	{
		system("sleep 0.5");
	}
	cout << "Call ended." << endl;

	// End, wait for mmdump to push a new session
	// And the function is called from the very beginning again
}

bool MMOperate::selectAnswer(int answer, bool isImage)
{
	system("screencap -p /sdcard/mmdump_scap.png");
	vector<unsigned char> image; //the raw pixels
	unsigned int width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, "/sdcard/mmdump_scap.png");
	//if there's an error, display it
	if(error) cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA...

	cx = width;
	cy = height;

	int scan_offset = isImage ? width*0.1 : width*SCAN_POS;
	scan_offset*=4;
	int scan_row = width*4;
	int scan_start = isImage ? scan_row*(SCAN_IMG*height) : scan_row*(SCAN_START*height);
	int scan_end = scan_row*(SCAN_END*height);

	bool isInSelection = false;
	vector<int> positions;
	for (int i = scan_start+scan_offset; i < scan_end+scan_offset; i+=scan_row)
	{
		if (isInSelection)
		{
			// Look for the end of the sel area
			if (abs(image[i]-BG_R)<TOLERANCE && abs(image[i+1]-BG_G)<TOLERANCE && abs(image[i+2]-BG_B)<TOLERANCE)
			{
				isInSelection = false;
				int beg = positions.back();
				positions.pop_back();
				positions.push_back((beg+i)/2/4/width); // Map array index to screen coordinate
			}
		} else
		{
			// Look for sel area
			if (abs(image[i]-CANDIDATE_R)<TOLERANCE && abs(image[i+1]-CANDIDATE_G)<TOLERANCE && abs(image[i+2]-CANDIDATE_B)<TOLERANCE)
			{
				positions.push_back(i);
				isInSelection = true;
			}

			if (abs(image[i]-CORRECT_R)<TOLERANCE && abs(image[i+1]-CORRECT_G)<TOLERANCE && abs(image[i+2]-CORRECT_B)<TOLERANCE)
			{
				return false;
			}
			if (abs(image[i]-INCORRECT_R)<TOLERANCE && abs(image[i+1]-INCORRECT_G)<TOLERANCE && abs(image[i+2]-INCORRECT_B)<TOLERANCE)
			{
				return false;
			}
			if (abs(image[i]-SELECTED_R)<TOLERANCE && abs(image[i+1]-SELECTED_G)<TOLERANCE && abs(image[i+2]-SELECTED_B)<TOLERANCE)
			{
				return false;
			}


		}
	}

	if (isImage)
	{
		// scan second colume
		// This scan does check mostly
		scan_offset = width*0.9;
		scan_offset *= 4;
		scan_start = scan_row*(SCAN_START*height);

		for (int i = scan_start+scan_offset; i < scan_end+scan_offset; i+=scan_row)
		{
			if (isInSelection)
			{
				// Look for the end of the sel area
				if (abs(image[i]-BG_R)<TOLERANCE && abs(image[i+1]-BG_G)<TOLERANCE && abs(image[i+2]-BG_B)<TOLERANCE)
				{
					isInSelection = false;
					int beg = positions.back();
					positions.pop_back();
					positions.push_back((beg+i)/2/4/width); // Map array index to screen coordinate
				}
			} else
			{
				// Look for sel area
				if (abs(image[i]-CANDIDATE_R)<TOLERANCE && abs(image[i+1]-CANDIDATE_G)<TOLERANCE && abs(image[i+2]-CANDIDATE_B)<TOLERANCE)
				{
					positions.push_back(i);
					isInSelection = true;
				}

				if (abs(image[i]-CORRECT_R)<TOLERANCE && abs(image[i+1]-CORRECT_G)<TOLERANCE && abs(image[i+2]-CORRECT_B)<TOLERANCE)
				{
					return false;
				}
				if (abs(image[i]-INCORRECT_R)<TOLERANCE && abs(image[i+1]-INCORRECT_G)<TOLERANCE && abs(image[i+2]-INCORRECT_B)<TOLERANCE)
				{
					return false;
				}
				if (abs(image[i]-SELECTED_R)<TOLERANCE && abs(image[i+1]-SELECTED_G)<TOLERANCE && abs(image[i+2]-SELECTED_B)<TOLERANCE)
				{
					return false;
				}


			}
		}
	}

	if (positions.size()>=4 && isImage)
	{
		// Image Question
		switch(answer)
		{
		case 0:
			tap(width/8+rand()%(width/4),positions[0]);
			break;
		case 1:
			tap(width*5/8+rand()%(width/4),positions[0]);
			break;
		case 2:
			tap(width/8+rand()%(width/4),positions[1]);
			break;
		case 3:
			tap(width*5/8+rand()%(width/4),positions[1]);
			break;
		}
		return true;
	}

	if (positions.size()==4)
	{
		// Regular question
		tap(width/4+rand()%(width/2),positions[answer]);
		return true;
	}
	return false;
}

bool MMOperate::dismissAd()
{
	tap(cx/2, cy*AD_DISMISS);
	system("screencap -p /sdcard/mmdump_scap.png");
	vector<unsigned char> image; //the raw pixels
	unsigned int width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, "/sdcard/mmdump_scap.png");
	//if there's an error, display it
	if(error) cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA...

	int scan_offset = width*0.5;
	scan_offset*=4;
	int scan_row = width*4;
	int scan_start = 0;
	int scan_end = scan_row*height;

	int count = 0, match_count = 0;
	for (int i = scan_start+scan_offset; i < scan_end+scan_offset; i+=scan_row)
	{
		count++;
		if (abs(image[i]-RESULT_R)<TOLERANCE && abs(image[i+1]-RESULT_G)<TOLERANCE && abs(image[i+2]-RESULT_B)<TOLERANCE)
		{
			match_count++;
		}
	}
	if (match_count>count/10)
	{
		// The screen IS result, neither ad nor last question
		tap(width/8, height*MATCH_NEXT);
		return true;
	}

	return false;
}

bool MMOperate::startSession()
{
	system("screencap -p /sdcard/mmdump_scap.png");
	vector<unsigned char> image; //the raw pixels
	unsigned int width, height;

	//decode
	unsigned error = lodepng::decode(image, width, height, "/sdcard/mmdump_scap.png");
	//if there's an error, display it
	if(error) cout << "decoder error " << error << ": " << lodepng_error_text(error) << endl;
	//the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA...

	int scan_offset = width*0.9; // Scan at 3/4 position
	scan_offset *= 4;
	int scan_row = width*4;
	int scan_start = 0.12*height;
	scan_start*=scan_row;
	int scan_end = scan_row*(0.9*height);

	bool isInSelection = false;
	vector<int> positions;

	// Exclude waiting screen misdetection
	int det_index = width*(height*0.9)+width/2;
	det_index *= 4;
	if (abs(image[det_index]-WAITING_R)<TOLERANCE && abs(image[det_index+1]-WAITING_G)<TOLERANCE && abs(image[det_index+2]-WAITING_B)<TOLERANCE)
		return false;
	if (abs(image[det_index]-INCORRECT_R)<TOLERANCE && abs(image[det_index+1]-INCORRECT_G)<TOLERANCE && abs(image[det_index+2]-INCORRECT_B)<TOLERANCE)
		return false;

	for (int i = scan_start+scan_offset; i < scan_end+scan_offset; i+=scan_row)
	{
		if (isInSelection)
		{
			// Look for the end of the sel area
			if (abs(image[i]-BG_R)<TOLERANCE && abs(image[i+1]-BG_G)<TOLERANCE && abs(image[i+2]-BG_B)<TOLERANCE)
			{
				isInSelection = false;
				int beg = positions.back();
				positions.pop_back();
				positions.push_back((beg+i)/2/4/width); // Map array index to screen coordinate
			}
		} else
		{
			// Look for sel area
			if (abs(image[i]-CANDIDATE_R)<TOLERANCE && abs(image[i+1]-CANDIDATE_G)<TOLERANCE && abs(image[i+2]-CANDIDATE_B)<TOLERANCE)
			{
				positions.push_back(i);
				isInSelection = true;

			}
			if (abs(image[i]-SELECTED_R)<TOLERANCE && abs(image[i+1]-SELECTED_G)<TOLERANCE && abs(image[i+2]-SELECTED_B)<TOLERANCE)
			{
				// Something is already selected!
				cout << "Unexpected selection." << endl;
				return true;
			}
		}
	}

	if (positions.size()==7)
	{
		tap(width/4+rand()%(width/2),positions[rand()%7]);
		return true;
	}

	return false;
}

void MMOperate::tap(int x, int y)
{
	// Output tap event
	char buffer[32];
	sprintf(buffer, "input tap %d %d", x, y);
	system(buffer);
}
