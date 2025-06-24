# Makefile untuk Logging FUSE File System

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_FILE_OFFSET_BITS=64
LIBS = -lfuse -lpthread
TARGET = logging_fs
SOURCE = logging_fs.c

# Default target
all: $(TARGET)

# Compile the main program
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LIBS)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y libfuse-dev fuse

# Install dependencies (CentOS/RHEL/Fedora)
install-deps-fedora:
	sudo yum install -y fuse-devel fuse
	# atau untuk sistem yang lebih baru:
	# sudo dnf install -y fuse-devel fuse

# Create test environment
test-setup:
	mkdir -p test_target test_mount logs
	echo "Test file 1" > test_target/file1.txt
	echo "Test file 2" > test_target/file2.txt
	mkdir -p test_target/subdir
	echo "Subdirectory file" > test_target/subdir/subfile.txt

# Run the filesystem (foreground mode for testing)
run-test: $(TARGET) test-setup
	./$(TARGET) test_target logs/filesystem.log test_mount -f

# Run the filesystem (background mode)
run-daemon: $(TARGET)
	./$(TARGET) test_target logs/filesystem.log test_mount -o allow_other

# Unmount the filesystem
unmount:
	fusermount -u test_mount

# Clean up
clean:
	rm -f $(TARGET)
	rm -rf test_target test_mount logs

# Test the filesystem operations
test-operations: test-setup
	@echo "Testing filesystem operations..."
	@echo "1. Listing files:"
	ls -la test_mount/
	@echo "2. Reading a file:"
	cat test_mount/file1.txt
	@echo "3. Creating a new file:"
	echo "New test file" > test_mount/newfile.txt
	@echo "4. Creating a directory:"
	mkdir test_mount/newdir
	@echo "5. Copying a file:"
	cp test_mount/file2.txt test_mount/newdir/
	@echo "6. Checking log file:"
	tail -20 logs/filesystem.log

# Show help
help:
	@echo "Available targets:"
	@echo "  all             - Compile the program"
	@echo "  install-deps    - Install FUSE development libraries (Ubuntu/Debian)"
	@echo "  install-deps-fedora - Install FUSE development libraries (Fedora/CentOS)"
	@echo "  test-setup      - Create test directories and files"
	@echo "  run-test        - Run filesystem in foreground mode for testing"
	@echo "  run-daemon      - Run filesystem in background mode"
	@echo "  test-operations - Test various file operations"
	@echo "  unmount         - Unmount the filesystem"
	@echo "  clean           - Remove compiled files and test directories"
	@echo "  help            - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make install-deps"
	@echo "  make all"
	@echo "  make run-test"
	@echo "  # In another terminal:"
	@echo "  make test-operations"

.PHONY: all install-deps install-deps-fedora test-setup run-test run-daemon unmount clean test-operations help