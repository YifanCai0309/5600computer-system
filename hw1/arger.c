#include <stdio.h>

// Convert the entire string to uppercase
void to_uppercase(char *text) {
  while (*text) {
    if (*text >= 'a' && *text <= 'z') {
      *text = *text - 'a' + 'A';
    }
    text++;
  }
}

// Convert the entire string to lowercase
void to_lowercase(char *text) {
  while (*text) {
    if (*text >= 'A' && *text <= 'Z') {
      *text = *text - 'A' + 'a';
    }
    text++;
  }
}

// Capitalize the first letter of each word
void capitalize_words(char *text) {
  int in_word = 0;
  while (*text) {
    if (*text == ' ' || *text == '\t' || *text == '\n') {
      putchar(*text);
      in_word = 0;
    } else {
      if (!in_word) {
        putchar(*text >= 'a' && *text <= 'z' ? *text - 'a' + 'A' : *text);
        in_word = 1;
      } else {
        putchar(*text >= 'A' && *text <= 'Z' ? *text - 'A' + 'a' : *text);
      }
    }
    text++;
  }
}

// Manual implementation of strcmp
int my_strcmp(const char *str1, const char *str2) {
  while (*str1 && *str2) {
    if (*str1 != *str2) {
      return (*str1 - *str2);
    }
    str1++;
    str2++;
  }
  return (*str1 - *str2);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <option> <text>\n", argv[0]);
    return -1;
  }

  // Ensure we have the correct number of arguments
  if (argc < 3) {
    printf("Error: Missing text to process.\n");
    return -1;
  }

  // Extract option and text
  const char *option = argv[1];
  const char *text = argv[2];

  // Check and perform actions based on the option
  if (my_strcmp(option, "-u") == 0) {
    // Convert to uppercase
    for (int i = 0; text[i]; i++) {
      putchar(text[i] >= 'a' && text[i] <= 'z' ? text[i] - 'a' + 'A' : text[i]);
    }
    putchar('\n');
  } else if (my_strcmp(option, "-l") == 0) {
    // Convert to lowercase
    for (int i = 0; text[i]; i++) {
      putchar(text[i] >= 'A' && text[i] <= 'Z' ? text[i] - 'A' + 'a' : text[i]);
    }
    putchar('\n');
  } else if (my_strcmp(option, "-cap") == 0) {
    // Capitalize the first letter of each word
    int inWord = 0;
    for (int i = 0; text[i]; i++) {
      if (text[i] == ' ' || text[i] == '\t') {
        putchar(text[i]);
        inWord = 0;
      } else {
        if (!inWord) {
          putchar(text[i] >= 'a' && text[i] <= 'z' ? text[i] - 'a' + 'A'
                                                   : text[i]);
          inWord = 1;
        } else {
          putchar(text[i] >= 'A' && text[i] <= 'Z' ? text[i] - 'A' + 'a'
                                                   : text[i]);
        }
      }
    }
    putchar('\n');
  } else {
    printf("Error: Invalid option '%s'.\n", option);
    return -1;
  }

  return 0;
}
