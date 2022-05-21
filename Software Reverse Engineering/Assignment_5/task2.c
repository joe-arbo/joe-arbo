#include <unistd.h>
#include <stdbool.h>
#include <string.h>

int val(int comp,char *app)
{
  int sum;
  int i;
  
  sum = 0;
  for (i = 0; i < comp; i = i + 1) {
    sum = sum + app[i];
  }
  return sum;
}


int res(int uwu,int uwu2)
{
  bool bVar1;
  
  bVar1 = (uwu + 1) - uwu2 != 1;
  if (bVar1) {
    puts("You failed!!!!!!!!");
  }
  else {
    puts("You have unlocked the code: Congratulations!!!!");
  }
  return (int)bVar1;
}


int main(int argc,char **argv)
{
  long lVar1;
  char *app;
  int uwu;
  int uwu2;
  size_t sVar2;
  long in_FS_OFFSET;
  int c1;
  int d1;
  char pass [33];
  
  lVar1 = *(long *)(in_FS_OFFSET + 0x28);
  if (argc == 2) {
    pass[0] = 0x48426a50434a7a77;
    pass[8] = 0x6162486b41487342;
    pass[16] = 0x7a6c666459686d7a;
    pass[24] = 0x455550706168644c;
    pass[32] = '\0';
    sVar2 = strlen(pass);
    uwu = val((int)sVar2,pass);
    app = argv[1];
    sVar2 = strlen(argv[1]);
    uwu2 = val((int)sVar2,app);
    res(uwu,uwu2);
  }
  else {
    puts("Not enough args");
  }
  if (lVar1 != *(long *)(in_FS_OFFSET + 0x28)) {
    /* WARNING: Subroutine does not return */
    __stack_chk_fail();
  }
  return 0;
}

//HBjPCJzwabHkAHsBzlfdYhmzEUPpahdL
//0x48426a50434a7a77, 0x6162486b41487342, 0x7a6c666459686d7a, 0x455550706168644c
/**
 * H H H 
 * B B 
 * j
 * P P
 * C
 * J
 * z z z
 * w
 * a a
 * b
 * k
 * A
 * s
 * l
 * f
 * d d
 * Y
 * h h
 * m
 * E
 * U
 * p
 * L
 */