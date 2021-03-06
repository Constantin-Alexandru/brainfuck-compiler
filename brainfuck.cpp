#include <iostream>
#include <fstream>
#include <cstring>
#include <conio.h>
#include <string>
#include <vector>

#define mem_size 30000

bool validFileExt(const char *filename);
bool readProgram(const char *filename, std::string &program);
bool isValid(std::string program);
void debug(const char *filename, unsigned char mem_arr[], int mem_pos, std::string program, int pos);
void interpret(const char *filename);
void compile_windows(const char *filename, std::string program);
void compile_linux(const char *filename, std::string program);
void compile(const char *filename);

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "ERROR: NOT ENOUGH ARGUMENTS\n";
    return -1;
  }

  if (std::strcmp(argv[1], "int") == 0)
  {
    interpret(argv[2]);

    return 0;
  }

  if (std::strcmp(argv[1], "com") == 0)
  {
    compile(argv[2]);

    return 0;
  }

  std::cout << "ERROR: INVALID COMMAND " << argv[1] << "\nAvailable commands are:\n    - int <filename>.bf: interprets the given brainfuck file \n    - com <filename>.bf: compiles the given brainfuck file\n";

  return -1;
}

bool validFileExt(const char *filename)
{
  int extStart = std::strlen(filename) - 3;

  if (std::strcmp(filename + extStart, ".bf") == 0)
    return true;

  return false;
}

bool readProgram(const char *filename, std::string &program)
{
  if (!validFileExt(filename))
  {
    return false;
  }

  std::ifstream in(filename);

  if (!in.good())
    return false;

  while (!in.eof())
  {
    std::string line;

    in >> line;

    program += line;
  }

  return true;
}

bool isValid(std::string program)
{
  int openBrackets = 0;
  int closeBrackets = 0;
  for (int i = 0; i < program.size(); i++)
  {
    if (program[i] == '[')
    {
      openBrackets++;
    }
    else if (program[i] == ']')
    {
      closeBrackets++;
    }
  }

  return openBrackets == closeBrackets;
}

void debug(const char *filename, unsigned char mem_arr[], int mem_pos, std::string program, int pos)
{

  int startMemPos = std::max(mem_pos - 20, 0);

  std::cout << filename << ":" << startMemPos << '\n';

  for (int i = startMemPos; i < startMemPos + 20; i++)
  {
    std::cout << (int)mem_arr[i] << " ";
  }

  std::string mem_pos_arr;

  std::cout << '\n';

  for (int i = 0; i < 20; i++)
  {
    if (i == (mem_pos - startMemPos))
    {
      std::cout << "^ (" << mem_pos << ")";
    }
    else
      std::cout << "  ";
  }

  std::cout << '\n';

  int startProgPos = std::max(pos - 19, 0);

  for (int i = startProgPos; i < startProgPos + 20; i++)
  {
    std::cout << program[i] << " ";
  }

  std::string prog_pos_arr;

  std::cout << '\n';

  for (int i = 0; i < 20; i++)
  {
    if (i == (pos - startProgPos))
    {
      std::cout << "^ ";
    }
    else
      std::cout << "  ";
  }

  std::cout << '\n';

  // getch();
}

void interpret(const char *filename)
{
  std::string program;
  if (!readProgram(filename, program))
  {
    std::cout << "ERROR: Invalid file " << filename << '\n';
    return;
  }

  if (!isValid(program))
  {
    std::cout << "ERROR: Invalid program in file " << filename << '\n';
    return;
  }

  std::vector<int> openLoops;

  unsigned char mem_arr[mem_size] = {0};
  int mem_pos = 0;
  int i;
  for (i = 0; i < program.size(); i++)
  {

    if (program[i] == '>')
    {
      mem_pos++;

      if (mem_pos >= mem_size)
        mem_pos -= mem_size;
    }
    else if (program[i] == '<')
    {
      mem_pos--;

      if (mem_pos < 0)
      {
        mem_pos += mem_size;
      }
    }
    else if (program[i] == '+')
    {
      mem_arr[mem_pos]++;
    }
    else if (program[i] == '-')
    {
      mem_arr[mem_pos]--;
    }
    else if (program[i] == '.')
    {
      std::cout << mem_arr[mem_pos];
    }
    else if (program[i] == ',')
    {
      mem_arr[mem_pos] = getchar();
    }
    else if (program[i] == '[')
    {
      if (mem_arr[mem_pos] == '\0')
      {
        int openCounter = 1;
        do
        {
          i++;
          if (program[i] == '[')
          {
            openCounter++;
          }
          else if (program[i] == ']')
          {
            openCounter--;
          }
        } while (openCounter != 0);
      }
    }
    else if (program[i] == ']')
    {
      if (mem_arr[mem_pos] != '\0')
      {
        int openCounter = 0;

        do
        {
          if (program[i] == '[')
          {
            openCounter++;
          }
          else if (program[i] == ']')
          {
            openCounter--;
          }
          i--;
        } while (openCounter != 0);
      }
    }
  }
}

void compile_windows(const char *filename, std::string program)
{
  std::string file(filename);

  file.pop_back();
  file.pop_back();
  file.pop_back();

  file += ".s";

  int loopCounter = 0;

  std::vector<int> openLoops;

  std::ofstream out(file);

  out << "    .global main\n\n";

  out << "    .text\n\n";

  out << "main:\n\n";

  out << "    # Allocating the array #\n";
  out << "    pushq %rbp\n";
  out << "    movq %rsp, %rbp\n";
  out << "    subq $16, %rsp\n";
  out << "    movl $30000, %edi\n";
  out << "    call malloc\n";
  out << "    movq %rax, -8(%rbp)\n";

  for (int i = 0; i < program.size(); i++)
  {
    if (program[i] == '>')
    {
      out << "    addq $1, -8(%rbp)\n";
    }
    else if (program[i] == '<')
    {
      out << "    subq $1, -8(%rbp)\n";
    }
    else if (program[i] == '+')
    {
      out << "    movq -8(%rbp), %rax\n";
      out << "    movzbl (%rax), %eax\n";
      out << "    addl $1, %eax\n";
      out << "    movl %eax, %edx\n";
      out << "    movq -8(%rbp), %rax\n";
      out << "    movb %dl, (%rax)\n";
    }
    else if (program[i] == '-')
    {
      out << "    movq -8(%rbp), %rax\n";
      out << "    movzbl (%rax), %eax\n";
      out << "    subl $1, %eax\n";
      out << "    movl %eax, %edx\n";
      out << "    movq -8(%rbp), %rax\n";
      out << "    movb %dl, (%rax)\n";
    }
    else if (program[i] == '.')
    {
      out << "    movq -8(%rbp), %rax\n";
      out << "    movzbl (%rax), %eax\n";
      out << "    movsbl %bl, %eax\n";
      out << "    movl %eax, %edi\n";
      out << "    call putchar\n";
    }
    else if (program[i] == ',')
    {
      out << "    call getchar\n";
      out << "    movl  %eax, %edx\n";
      out << "    movq -8(%rbp), %rax";
      out << "    movb %dl, (%rax)\n";
    }
    else if (program[i] == '[')
    {

      out << ".L" << loopCounter << ":\n";
      out << "    movq -8(%rbp), %rax\n";
      out << "    movzbl (%rax), %eax\n";
      out << "    testb %al, %al\n";
      out << "    je .L" << loopCounter << "end\n";

      openLoops.push_back(loopCounter);

      loopCounter++;
    }
    else if (program[i] == ']')
    {
      out << ".L" << openLoops.back() << "end:\n";
      out << "    movq -8(%rbp), %rax\n";
      out << "    movzbl (%rax), %eax\n";
      out << "    testb %al, %al\n";
      out << "    jne .L" << openLoops.back() << '\n';

      openLoops.pop_back();
    }
  }

  out << "    # Freeing the memory occupied by the array #";
  out << "    movq -8(%rbp), %rax\n";
  out << "    movq %rax, %rdi\n";
  out << "    call free\n";

  out.close();

  std::string asCmd = "as --64 -o out.o " + file;

  file.pop_back();
  file.pop_back();

  file += ".exe";

  std::string gccCmd = "g++ -o " + file + " out.o";

  system(asCmd.c_str());
  system(gccCmd.c_str());
}

void compile_linux(const char *filename, std::string program)
{
}

void compile(const char *filename)
{
  std::string program;
  if (!readProgram(filename, program))
  {
    std::cout << "ERROR: Invalid file " << filename << '\n';
    return;
  }

  if (!isValid(program))
  {
    std::cout << "ERROR: Invalid program in file " << filename << '\n';
    return;
  }

#ifdef _WIN32 || _WIN64
  compile_windows(filename, program);
#elif __linux__
  compile_linux(filename, program);
#endif
}