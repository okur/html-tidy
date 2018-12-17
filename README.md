# html-tidy

int get_filename_ext(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot) return 0;
    if(!strcmp(dot, ".html")) return 1;
    else return 0;
}
