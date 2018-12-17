#include <tidy.h>
#include <buffio.h>
#include <stdio.h>
#include <errno.h>

void tidy(char * buf){
  TidyBuffer output = {0};
  TidyDoc Doc = tidyCreate();
  tidyLoadConfig(Doc, "config.txt");
  tidyParseString(Doc, buf); 
  tidyCleanAndRepair(Doc);
  tidySaveBuffer(Doc, &output);
  strcpy(buf, output.bp);
  tidyBufFree(&output);
  tidyRelease(Doc);
  return;
}


int main(int argc, char **argv )
{
  char* buf = malloc(2000*sizeof(char));
  strcpy(buf,"<title>Foo</title><p>Foo!");
  tidy(buf);
  return 0;
}
