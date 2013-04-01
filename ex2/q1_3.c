int sub_process_file(FILE* filePtr, size_t fileSize) {
	char buffer[256];
	int i = 0;
	memset(buffer, 0, 256);
	printf("Processing...\n");
	fread(buffer, 1, fileSize, filePtr);
	for (; i != 256 && i != 0xDEADBEEF; i++) {
		if ('A' <= buffer[i] && buffer[i] <= 'Z') {
			buffer[i] = buffer[i] - ('a'-'A'); // to lowercase
		}
		if ('a' <= buffer[i] && buffer[i] <= 'z') {
			buffer[i] = buffer[i] + 2;
			if (buffer[i] <= 'z')
				continue;
			buffer[i] = buffer[i] - 26; // if > z make < z
		} else {
			if (buffer[i] == '{')
				buffer[i] = '[';
		}
	}
	printf("I think that the output is: %s\n", buffer);
}
