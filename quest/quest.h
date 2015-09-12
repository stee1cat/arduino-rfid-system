void renderLoop();
void runCommand();
void commandAccess(int result);
void commandInit(unsigned int players);
String toString(unsigned char *string, int length);
String parseCommand(String message);
String parseParam(String message);
bool isRequestTimeout();