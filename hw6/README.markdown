# Polybius Cipher Project

This project involves generating random text strings, storing them in a queue, and encrypting them using the Polybius cipher.

## Project Structure

- **`encryptor.c`**: Handles encryption tasks using the Polybius cipher.
- **`polybius.c`** and **`polybius.h`**: Implements the Polybius cipher.
- **`queue.c`** and **`queue.h`**: Defines a queue data structure for storing words.
- **`randString.c`**: Generates random text strings and writes them to `output.txt`.
- **`storeWord.c`**: Reads and stores words from `output.txt` into a queue.

## Compilation

To compile all components, run `make`.

## Running the Programs

### 1. **Random String Generation** (`randString`)
Generates random text strings and writes them to `output.txt`.

**Usage**: `./randString <number_of_strings> <file_name> [-a]`

Note: The `file_name` should be hardcoded to `output.txt`. The `<number_of_strings>` must be greater than 10,000 for testing.

**Example**: `./randString 10000 output.txt`

### 2. **Queue and Word Storage** (`storeWord`)
Reads words from `output.txt`, stores them in a queue, and prints each word.

**Usage**: `./storeWord`

### 3. **Polybius Cipher Encryption** (`encryptor`)
Encrypts 100 strings at a time using the Polybius cipher.

**Usage**: `./encryptor output.txt`

The program processes `output.txt` in batches, ensuring each batch writes to a distinct file.

## Cleanup

To clean up compiled binaries and object files, run `make clean`.

## Workflow Summary

1. Generate random strings: `./randString 10000 output.txt`
2. Store words using a queue: `./storeWord`
3. Encrypt using the Polybius cipher: `./encryptor output.txt`

## Edge Cases Handled
- Input file is empty.
- Missing cipher programs.

Ensure the project is tested with large datasets (>10,000 strings) to validate behavior under stress.
