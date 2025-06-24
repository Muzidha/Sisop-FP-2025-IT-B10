#!/bin/bash



set -e  

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

TARGET_DIR="./test_target"
MOUNT_DIR="./test_mount"
LOG_FILE="./logs/filesystem.log"
PROGRAM="./logging_fs"


print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_step() {
    echo -e "${BLUE}[STEP]${NC} $1"
}


check_program() {
    if [ ! -f "$PROGRAM" ]; then
        print_error "Program $PROGRAM tidak ditemukan. Compile terlebih dahulu dengan 'make all'"
        exit 1
    fi
}


setup_test_env() {
    print_step "Setting up test environment..."
    
    mkdir -p "$TARGET_DIR" "$MOUNT_DIR" "$(dirname "$LOG_FILE")"
    
    
    echo "This is test file 1" > "$TARGET_DIR/file1.txt"
    echo "This is test file 2" > "$TARGET_DIR/file2.txt"
    echo "This is a large test file with more content for testing read operations" > "$TARGET_DIR/large_file.txt"
    
    
    mkdir -p "$TARGET_DIR/subdir1"
    echo "File in subdirectory 1" > "$TARGET_DIR/subdir1/sub_file1.txt"
    
    mkdir -p "$TARGET_DIR/subdir2"
    echo "File in subdirectory 2" > "$TARGET_DIR/subdir2/sub_file2.txt"
    
    print_status "Test environment created successfully"
}


start_filesystem() {
    print_step "Starting logging filesystem..."
    
    
    if mountpoint -q "$MOUNT_DIR" 2>/dev/null; then
        print_warning "Filesystem already mounted. Unmounting first..."
        fusermount -u "$MOUNT_DIR" 2>/dev/null || true
        sleep 1
    fi
    
    
    "$PROGRAM" "$TARGET_DIR" "$LOG_FILE" "$MOUNT_DIR" &
    FUSE_PID=$!
    
    
    sleep 2
    
    
    if ! mountpoint -q "$MOUNT_DIR" 2>/dev/null; then
        print_error "Failed to mount filesystem"
        kill $FUSE_PID 2>/dev/null || true
        exit 1
    fi
    
    print_status "Filesystem mounted successfully (PID: $FUSE_PID)"
}


test_operations() {
    print_step "Testing file system operations..."
    
    echo "========================= TESTING FILE OPERATIONS ========================="
    
    # Test 1: List directory
    print_step "Test 1: Listing root directory"
    ls -la "$MOUNT_DIR/"
    
    # Test 2: Read files
    print_step "Test 2: Reading files"
    echo "Content of file1.txt:"
    cat "$MOUNT_DIR/file1.txt"
    echo "Content of large_file.txt:"
    head -c 50 "$MOUNT_DIR/large_file.txt"
    echo "..."
    
    # Test 3: Create new file
    print_step "Test 3: Creating new file"
    echo "This is a newly created file" > "$MOUNT_DIR/new_file.txt"
    
    # Test 4: Create directory
    print_step "Test 4: Creating new directory"
    mkdir "$MOUNT_DIR/new_directory"
    
    # Test 5: Copy file
    print_step "Test 5: Copying file to new directory"
    cp "$MOUNT_DIR/file2.txt" "$MOUNT_DIR/new_directory/"
    
    # Test 6: Move/rename file
    print_step "Test 6: Renaming file"
    mv "$MOUNT_DIR/new_file.txt" "$MOUNT_DIR/renamed_file.txt"
    
    # Test 7: Change permissions
    print_step "Test 7: Changing file permissions"
    chmod 644 "$MOUNT_DIR/renamed_file.txt"
    
    # Test 8: Append to file
    print_step "Test 8: Appending to file"
    echo "This line is appended" >> "$MOUNT_DIR/renamed_file.txt"
    
    # Test 9: Create and remove file
    print_step "Test 9: Creating and removing file"
    touch "$MOUNT_DIR/temp_file.txt"
    rm "$MOUNT_DIR/temp_file.txt"
    
    # Test 10: Read subdirectory
    print_step "Test 10: Listing subdirectory"
    ls -la "$MOUNT_DIR/subdir1/"
    
    # Test 11: Access subdirectory file
    print_step "Test 11: Reading subdirectory file"
    cat "$MOUNT_DIR/subdir1/sub_file1.txt"
    
    print_status "All test operations completed"
}

# Function to show log file
show_logs() {
    print_step "Displaying filesystem log..."
    
    if [ -f "$LOG_FILE" ]; then
        echo "========================= FILESYSTEM LOG ========================="
        cat "$LOG_FILE"
        echo "================================================================"
        
        # Show summary
        echo ""
        print_step "Log Summary:"
        echo "Total operations logged: $(wc -l < "$LOG_FILE")"
        echo "OPEN operations: $(grep -c "OPEN:" "$LOG_FILE" || echo "0")"
        echo "READ operations: $(grep -c "READ:" "$LOG_FILE" || echo "0")"
        echo "WRITE operations: $(grep -c "WRITE:" "$LOG_FILE" || echo "0")"
        echo "MKDIR operations: $(grep -c "MKDIR:" "$LOG_FILE" || echo "0")"
        echo "CREATE operations: $(grep -c "CREATE:" "$LOG_FILE" || echo "0")"
        echo "RENAME operations: $(grep -c "RENAME:" "$LOG_FILE" || echo "0")"
        echo "UNLINK operations: $(grep -c "UNLINK:" "$LOG_FILE" || echo "0")"
    else
        print_warning "Log file not found: $LOG_FILE"
    fi
}

# Function to cleanup
cleanup() {
    print_step "Cleaning up..."
    
    # Unmount filesystem
    if mountpoint -q "$MOUNT_DIR" 2>/dev/null; then
        fusermount -u "$MOUNT_DIR" 2>/dev/null || true
        print_status "Filesystem unmounted"
    fi
    
    # Kill the FUSE process if still running
    if [ -n "${FUSE_PID:-}" ]; then
        kill $FUSE_PID 2>/dev/null || true
        print_status "FUSE process terminated"
    fi
    
    
}

# Function to show usage
show_usage() {
    echo "Usage: $0 [option]"
    echo "Options:"
    echo "  setup     - Setup test environment only"
    echo "  start     - Start filesystem only"
    echo "  test      - Run tests only (filesystem must be running)"
    echo "  logs      - Show logs only"
    echo "  full      - Run complete test (default)"
    echo "  cleanup   - Cleanup and unmount"
    echo "  help      - Show this help"
}

# Trap to ensure cleanup on exit
trap cleanup EXIT INT TERM

# Main execution
case "${1:-full}" in
    "setup")
        check_program
        setup_test_env
        ;;
    "start")
        check_program
        setup_test_env
        start_filesystem
        print_status "Filesystem is running. Use 'fusermount -u $MOUNT_DIR' to unmount"
        trap - EXIT  # Remove trap so filesystem keeps running
        ;;
    "test")
        test_operations
        ;;
    "logs")
        show_logs
        ;;
    "full")
        check_program
        setup_test_env
        start_filesystem
        sleep 1
        test_operations
        show_logs
        ;;
    "cleanup")
        cleanup
        ;;
    "help")
        show_usage
        ;;
    *)
        print_error "Unknown option: $1"
        show_usage
        exit 1
        ;;
esac

print_status "Script completed successfully!"