#include <tidy.h>
#include <buffio.h>
#include <stdio.h>
#include <errno.h>


void tidy(char * buf){
  TidyBuffer output = {0};
  TidyDoc Doc = tidyCreate();
  tidyOptSetBool(Doc, TidyXhtmlOut, yes);
  tidyOptSetInt(Doc, TidyIndentContent, 1);
  tidyOptSetInt(Doc, TidyIndentSpaces, 4);
  tidyParseString(Doc, buf); 
  tidyOptSetBool(Doc, TidyForceOutput, yes);  
  tidySaveBuffer(Doc, &output );
  strcpy(buf, output.bp);
  printf("%s", buf);
  printf("%d\n", output.size);
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
