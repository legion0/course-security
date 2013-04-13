nasm -f win32 messagebox.asm
gcc messagebox.obj -o messagebox.exe
rm messagebox.obj