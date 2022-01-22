#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <3ds.h>

Result http_post(const char* url, const char* data,bool checkRequest){
	Result ret=0;
	httpcContext context;
	char *newurl=NULL;
	u32 statuscode=0;
	u32 contentsize=0, readsize=0, size=0;
	u8 *buf, *lastbuf;
	if(checkRequest){
		printf("POSTing %s\n", url);
	}

	do {
		ret = httpcOpenContext(&context, HTTPC_METHOD_POST, url, 0);
		if(checkRequest){
			printf("return from httpcOpenContext: %" PRIx32 "\n",ret);
		}

		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
		if(checkRequest){
			printf("return from httpcSetSSLOpt: %" PRIx32 "\n",ret);
		}

		// Enable Keep-Alive connections
		ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
		if(checkRequest){
			printf("return from httpcSetKeepAlive: %" PRIx32 "\n",ret);
		}

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&context, "User-Agent", "httpc-example/1.0.0");
		if(checkRequest){
			printf("return from httpcAddRequestHeaderField: %" PRIx32 "\n",ret);
		}
		
		// Set a Content-Type header so websites can identify the format of our raw body data.
		// If you want to send form data in your request, use:
		//ret = httpcAddRequestHeaderField(&context, "Content-Type", "multipart/form-data");
		// If you want to send raw JSON data in your request, use:
		ret = httpcAddRequestHeaderField(&context, "Content-Type", "application/json");
		if(checkRequest){
			printf("return from httpcAddRequestHeaderField: %" PRIx32 "\n",ret);
		}

		// Post specified data.
		// If you want to add a form field to your request, use:
		//ret = httpcAddPostDataAscii(&context, "data", value);
		// If you want to add a form field containing binary data to your request, use:
		//ret = httpcAddPostDataBinary(&context, "field name", yourBinaryData, length);
		// If you want to add raw data to your request, use:
		ret = httpcAddPostDataRaw(&context, (u32*)data, strlen(data));
		if(checkRequest){
			printf("return from httpcAddPostDataRaw: %" PRIx32 "\n",ret);
		}
		

		ret = httpcBeginRequest(&context);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		ret = httpcGetResponseStatusCode(&context, &statuscode);
		if(ret!=0){
			httpcCloseContext(&context);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(&context);
				return -1;
			}
			ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
			url = newurl; // Change pointer to the url that we just learned
			printf("redirecting to url: %s\n",url);
			httpcCloseContext(&context); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		printf("URL returned status: %" PRIx32 "\n", statuscode);
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -2;
	}

	// This relies on an optional Content-Length header and may be 0
	ret=httpcGetDownloadSizeState(&context, NULL, &contentsize);
	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return ret;
	}
	if(checkRequest){
		printf("reported size: %" PRIx32 "\n",contentsize);		
	}

	// Start with a single page buffer
	buf = (u8*)malloc(0x1000);
	if(buf==NULL){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		return -1;
	}

	do {
		// This download loop resizes the buffer as data is read.
		ret = httpcDownloadData(&context, buf+size, 0x1000, &readsize);
		size += readsize; 
		if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING){
				lastbuf = buf; // Save the old pointer, in case realloc() fails.
				buf = realloc(buf, size + 0x1000);
				if(buf==NULL){ 
					httpcCloseContext(&context);
					free(lastbuf);
					if(newurl!=NULL) free(newurl);
					return -1;
				}
			}
	} while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);	

	if(ret!=0){
		httpcCloseContext(&context);
		if(newurl!=NULL) free(newurl);
		free(buf);
		return -1;
	}

	// Resize the buffer back down to our actual final size
	lastbuf = buf;
	buf = realloc(buf, size);
	if(buf==NULL){ // realloc() failed.
		httpcCloseContext(&context);
		free(lastbuf);
		if(newurl!=NULL) free(newurl);
		return -1;
	}

	if(checkRequest){
		printf("response size: %" PRIx32 "\n",size);	
	}

	// Print result
	printf((char*)buf);
	printf("\n");
	
	gfxFlushBuffers();
	gfxSwapBuffers();

	httpcCloseContext(&context);
	free(buf);
	if (newurl!=NULL) free(newurl);

	return 0;
}

void arrayToJson (char array[32][32], int sz, char string[1087]){
	char end[]="\"";
	char empty[]="";
	for (int i = 0; i < 32; i++)
	{
		char k[32]={"\""};
		strcat(k,array[i]);
		strcat(k,end);
		if(i==0){
			// without comma
			char space[]=" ";
			strcat(string,space);
			strcat(string,k);
		}else{
			// comma in the start
			char comma[]=", ";
			strcat(string,comma);
			strcat(string,k);
		}

		strcpy(array[i],empty);
	}
	strcat(string,"]");
}

static SwkbdCallbackResult MyCallback(void* user, const char** ppMessage, const char* text, size_t textlen){
	if (strstr(text, "lenny"))
	{
		*ppMessage = "Nice try but I'm not letting you use that meme right now";
		return SWKBD_CALLBACK_CONTINUE;
	}

	if (strstr(text, "brick"))
	{
		*ppMessage = "~Time to visit Brick City~";
		return SWKBD_CALLBACK_CLOSE;
	}

	return SWKBD_CALLBACK_OK;
}

char* getIp(){
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	printf("Press A to write the ip direction\n");
	printf("START: exit\n");

	static char* ip;

	while (aptMainLoop()){
		hidScanInput();

		u32 kDown = hidKeysDown();

		if (kDown & KEY_START)
			break;

		static SwkbdState swkbd;
		static char mybuf[60];
		static SwkbdStatusData swkbdStatus;
		static SwkbdLearningData swkbdLearning;
		SwkbdButton button = SWKBD_BUTTON_NONE;
		bool didit = false;

		if (kDown & KEY_A)
		{
			didit = true;
			swkbdInit(&swkbd, SWKBD_TYPE_WESTERN, 2, -1);
			swkbdSetValidation(&swkbd, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);
			swkbdSetFeatures(&swkbd, SWKBD_DARKEN_TOP_SCREEN | SWKBD_ALLOW_HOME | SWKBD_ALLOW_RESET | SWKBD_ALLOW_POWER);
			swkbdSetFilterCallback(&swkbd, MyCallback, NULL);

			bool shouldQuit = false;
			mybuf[0] = 0;
			do
			{
				swkbdSetInitialText(&swkbd, mybuf);
				button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
				if (button != SWKBD_BUTTON_NONE)
					break;

				SwkbdResult res = swkbdGetResult(&swkbd);
				if (res == SWKBD_RESETPRESSED)
				{
					shouldQuit = true;
					aptSetChainloaderToSelf();
					break;
				}
				else if (res != SWKBD_HOMEPRESSED && res != SWKBD_POWERPRESSED)
					break; // An actual error happened, display it

				shouldQuit = !aptMainLoop();
			} while (!shouldQuit);

			if (shouldQuit)
				break;
		}

		if (didit)
		{
			if (button != SWKBD_BUTTON_NONE)
			{
				ip=malloc(strlen(mybuf)+1);
				strcpy(ip, mybuf);

				// printf("You pressed button %d\n", button);
				// printf("Text: %s\n", url);
				// printf("tamanio url: %i\n",sizeof(url));
			} else
				printf("swkbd event: %d\n", swkbdGetResult(&swkbd));
			
			break;
		}

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();

		gspWaitForVBlank();
	}

	gfxExit();
	if(strlen(ip) > 4){
		return ip;
	}else{
		char* fail="url captation failed.";
		return fail;
	}
}

char* getUrl(char* ip, char* port, char* path){
	static char* result;
	result = malloc(1+strlen(path)+strlen(ip)+strlen(path));
	strcpy(result, "http://");
	strcat(result, ip);
	strcat(result, port);
	strcat(result, path);

	return result;
}

int main()
{
	char* ip_adress=getIp();
	char* url_init=getUrl(ip_adress,":3000","/api/init");
	char* url_keypressed= getUrl(ip_adress,":3000","/api/keyPressed");
	printf("url init: %s\n",url_init);
	printf("url key: %s\n",url_keypressed);

	char keysNames[32][32] = {
		"KEY_A", "KEY_B", "KEY_SELECT", "KEY_START",
		"KEY_DRIGHT", "KEY_DLEFT", "KEY_DUP", "KEY_DDOWN",
		"KEY_R", "KEY_L", "KEY_X", "KEY_Y",
		"", "", "KEY_ZL", "KEY_ZR",
		"", "", "", "",
		"KEY_TOUCH", "", "", "",
		"KEY_CSTICK_RIGHT", "KEY_CSTICK_LEFT", "KEY_CSTICK_UP", "KEY_CSTICK_DOWN",
		"KEY_CPAD_RIGHT", "KEY_CPAD_LEFT", "KEY_CPAD_UP", "KEY_CPAD_DOWN"
	};

	Result ret=0;
	gfxInitDefault();
	httpcInit(4 * 1024 * 1024); // Buffer size when POST/PUT.
	consoleInit(GFX_BOTTOM,NULL);

	ret=http_post(url_init, "{\"foo\":\"barr\"}",false);
	printf("return from http_post: %" PRIx32 "\n",ret);

	while (aptMainLoop())
	{
		gspWaitForVBlank();
		hidScanInput();

		u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0;
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();

		circlePosition pos;
		hidCircleRead(&pos);
		if ((kDown & KEY_START) && (kDown & KEY_SELECT)) break;

		// --- buttons registration ---
		// if a key is pressed and the keys are diferent from the previous frame
		if(kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld) {
			consoleClear();
			char helds[32][32];
			char downs[32][32];
			char ups[32][32];

			// keys tracking
			for (int i = 0; i < 32; i++)
			{
				if(kDown & BIT(i)){
					strcpy(downs[i],keysNames[i]);
					printf("%s down\n", downs[i]);
				}
				if(kHeld & BIT(i)){
					strcpy(helds[i],keysNames[i]);
					printf("%s held\n", helds[i]);
				}
				if(kUp & BIT(i)){					
					strcpy(ups[i],keysNames[i]);
					printf("%s up\n", ups[i]);
				}
			}
			
			char stringHelds[1087]={"["};
			arrayToJson(helds,32, stringHelds);

			char stringDowns[1087]={"["};
			arrayToJson(downs,32, stringDowns);

			char stringUps[1087]={"["};
			arrayToJson(ups,32, stringUps);

			

			char buf[3207];
			snprintf(buf, 3207,"{\"circlePad\":{\"x\":%d,\"y\":%d},\"keysDown\":%s,\"keysHeld\":%s,\"keysUp\":%s}",
				pos.dx,pos.dy,
				stringDowns,stringHelds,stringUps
			);
			ret = http_post(url_keypressed, buf,false);
			printf("%s",buf);
			printf("\nreturn from http_pos: %" PRIx32 "\n",ret);
		}
		kDownOld = kDown;
		kHeldOld = kHeld;
		kUpOld = kUp;
	}

	// Exit services
	httpcExit();
	gfxExit();
	return 0;
}

