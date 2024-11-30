TCP Remote File System

Compilation
-----------
make

Setup
-----
mkdir -p data
echo "Sample content" > data/localfoo.txt

Running the Server
------------------
./server

Client Commands
---------------
WRITE:
./client WRITE data/localfoo.txt folder/foo.txt

GET:
./client GET folder/foo.txt data/received_foo.txt

RM:
./client RM folder/foo.txt
./client RM folder
