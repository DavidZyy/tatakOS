
/home/zhuyangyang/PROJECT/2022_3/tatakOS/build/user_prog/_init:     file format elf64-littleriscv


Disassembly of section .text:

0000000000000000 <main>:

char *argv[] = { "sh", 0 };

int
main(void)
{
   0:	fe010113          	addi	sp,sp,-32
   4:	00113c23          	sd	ra,24(sp)
   8:	00813823          	sd	s0,16(sp)
   c:	00913423          	sd	s1,8(sp)
  10:	02010413          	addi	s0,sp,32
  int pid, wpid;


  // printf(red("init run!\n"));

  if(open("console", O_RDWR) < 0){
  14:	00200593          	li	a1,2
  18:	00001517          	auipc	a0,0x1
  1c:	bd050513          	addi	a0,a0,-1072 # be8 <timetag+0xc>
  20:	00001097          	auipc	ra,0x1
  24:	b5c080e7          	jalr	-1188(ra) # b7c <open>
  28:	06054863          	bltz	a0,98 <main+0x98>
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  2c:	00000513          	li	a0,0
  30:	00001097          	auipc	ra,0x1
  34:	b10080e7          	jalr	-1264(ra) # b40 <dup>
  dup(0);  // stderr
  38:	00000513          	li	a0,0
  3c:	00001097          	auipc	ra,0x1
  40:	b04080e7          	jalr	-1276(ra) # b40 <dup>

  for(;;){
    printf("init: starting sh\n");
  44:	00001517          	auipc	a0,0x1
  48:	bac50513          	addi	a0,a0,-1108 # bf0 <timetag+0x14>
  4c:	00000097          	auipc	ra,0x0
  50:	4e4080e7          	jalr	1252(ra) # 530 <printf>
    pid = fork();
  54:	00001097          	auipc	ra,0x1
  58:	a80080e7          	jalr	-1408(ra) # ad4 <fork>
  5c:	00050493          	mv	s1,a0
    if(pid < 0){
  60:	06054463          	bltz	a0,c8 <main+0xc8>
      printf("init: fork failed\n");
      exit(1);
    }
    if(pid == 0){
  64:	08050063          	beqz	a0,e4 <main+0xe4>
    }

    for(;;){
      // this call to wait() returns if the shell exits,
      // or if a parentless process exits.
      wpid = wait((int *) 0);
  68:	00000513          	li	a0,0
  6c:	00001097          	auipc	ra,0x1
  70:	a80080e7          	jalr	-1408(ra) # aec <wait>
      if(wpid == pid){
  74:	fca488e3          	beq	s1,a0,44 <main+0x44>
        // the shell exited; restart it.
        break;
      } else if(wpid < 0){
  78:	fe0558e3          	bgez	a0,68 <main+0x68>
        printf("init: wait returned an error\n");
  7c:	00001517          	auipc	a0,0x1
  80:	bc450513          	addi	a0,a0,-1084 # c40 <timetag+0x64>
  84:	00000097          	auipc	ra,0x0
  88:	4ac080e7          	jalr	1196(ra) # 530 <printf>
        exit(1);
  8c:	00100513          	li	a0,1
  90:	00001097          	auipc	ra,0x1
  94:	a50080e7          	jalr	-1456(ra) # ae0 <exit>
    mknod("console", CONSOLE, 0);
  98:	00000613          	li	a2,0
  9c:	00100593          	li	a1,1
  a0:	00001517          	auipc	a0,0x1
  a4:	b4850513          	addi	a0,a0,-1208 # be8 <timetag+0xc>
  a8:	00001097          	auipc	ra,0x1
  ac:	aec080e7          	jalr	-1300(ra) # b94 <mknod>
    open("console", O_RDWR);
  b0:	00200593          	li	a1,2
  b4:	00001517          	auipc	a0,0x1
  b8:	b3450513          	addi	a0,a0,-1228 # be8 <timetag+0xc>
  bc:	00001097          	auipc	ra,0x1
  c0:	ac0080e7          	jalr	-1344(ra) # b7c <open>
  c4:	f69ff06f          	j	2c <main+0x2c>
      printf("init: fork failed\n");
  c8:	00001517          	auipc	a0,0x1
  cc:	b4050513          	addi	a0,a0,-1216 # c08 <timetag+0x2c>
  d0:	00000097          	auipc	ra,0x0
  d4:	460080e7          	jalr	1120(ra) # 530 <printf>
      exit(1);
  d8:	00100513          	li	a0,1
  dc:	00001097          	auipc	ra,0x1
  e0:	a04080e7          	jalr	-1532(ra) # ae0 <exit>
      exec("sh", argv);
  e4:	00001597          	auipc	a1,0x1
  e8:	bf458593          	addi	a1,a1,-1036 # cd8 <argv>
  ec:	00001517          	auipc	a0,0x1
  f0:	b3450513          	addi	a0,a0,-1228 # c20 <timetag+0x44>
  f4:	00001097          	auipc	ra,0x1
  f8:	a28080e7          	jalr	-1496(ra) # b1c <exec>
      printf("init: exec sh failed\n");
  fc:	00001517          	auipc	a0,0x1
 100:	b2c50513          	addi	a0,a0,-1236 # c28 <timetag+0x4c>
 104:	00000097          	auipc	ra,0x0
 108:	42c080e7          	jalr	1068(ra) # 530 <printf>
      exit(1);
 10c:	00100513          	li	a0,1
 110:	00001097          	auipc	ra,0x1
 114:	9d0080e7          	jalr	-1584(ra) # ae0 <exit>

0000000000000118 <putc>:

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
 118:	fe010113          	addi	sp,sp,-32
 11c:	00113c23          	sd	ra,24(sp)
 120:	00813823          	sd	s0,16(sp)
 124:	02010413          	addi	s0,sp,32
 128:	feb407a3          	sb	a1,-17(s0)
  write(fd, &c, 1);
 12c:	00100613          	li	a2,1
 130:	fef40593          	addi	a1,s0,-17
 134:	00001097          	auipc	ra,0x1
 138:	a54080e7          	jalr	-1452(ra) # b88 <write>
}
 13c:	01813083          	ld	ra,24(sp)
 140:	01013403          	ld	s0,16(sp)
 144:	02010113          	addi	sp,sp,32
 148:	00008067          	ret

000000000000014c <printint>:

static void
printint(int fd, int xx, int base, int sgn)
{
 14c:	fd010113          	addi	sp,sp,-48
 150:	02113423          	sd	ra,40(sp)
 154:	02813023          	sd	s0,32(sp)
 158:	00913c23          	sd	s1,24(sp)
 15c:	01213823          	sd	s2,16(sp)
 160:	03010413          	addi	s0,sp,48
 164:	00050913          	mv	s2,a0
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 168:	00068463          	beqz	a3,170 <printint+0x24>
 16c:	0605c663          	bltz	a1,1d8 <printint+0x8c>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 170:	0005859b          	sext.w	a1,a1
  neg = 0;
 174:	00000813          	li	a6,0
  }

  i = 0;
 178:	00000493          	li	s1,0
  do{
    buf[i++] = digits[x % base];
 17c:	0006079b          	sext.w	a5,a2
 180:	02c5f6bb          	remuw	a3,a1,a2
 184:	00048513          	mv	a0,s1
 188:	0014849b          	addiw	s1,s1,1
 18c:	02069693          	slli	a3,a3,0x20
 190:	0206d693          	srli	a3,a3,0x20
 194:	00001717          	auipc	a4,0x1
 198:	b2470713          	addi	a4,a4,-1244 # cb8 <digits>
 19c:	00d70733          	add	a4,a4,a3
 1a0:	00074683          	lbu	a3,0(a4)
 1a4:	fe050713          	addi	a4,a0,-32
 1a8:	00870733          	add	a4,a4,s0
 1ac:	fed70823          	sb	a3,-16(a4)
  }while((x /= base) != 0);
 1b0:	0005871b          	sext.w	a4,a1
 1b4:	02c5d5bb          	divuw	a1,a1,a2
 1b8:	fcf772e3          	bgeu	a4,a5,17c <printint+0x30>
  if(neg)
 1bc:	04080063          	beqz	a6,1fc <printint+0xb0>
    buf[i++] = '-';
 1c0:	fe048793          	addi	a5,s1,-32
 1c4:	008784b3          	add	s1,a5,s0
 1c8:	02d00793          	li	a5,45
 1cc:	fef48823          	sb	a5,-16(s1)
 1d0:	0025049b          	addiw	s1,a0,2
 1d4:	0280006f          	j	1fc <printint+0xb0>
    x = -xx;
 1d8:	40b005bb          	negw	a1,a1
    neg = 1;
 1dc:	00100813          	li	a6,1
    x = -xx;
 1e0:	f99ff06f          	j	178 <printint+0x2c>

  while(--i >= 0)
    putc(fd, buf[i]);
 1e4:	fe048793          	addi	a5,s1,-32
 1e8:	008787b3          	add	a5,a5,s0
 1ec:	ff07c583          	lbu	a1,-16(a5)
 1f0:	00090513          	mv	a0,s2
 1f4:	00000097          	auipc	ra,0x0
 1f8:	f24080e7          	jalr	-220(ra) # 118 <putc>
  while(--i >= 0)
 1fc:	fff4849b          	addiw	s1,s1,-1
 200:	fe04d2e3          	bgez	s1,1e4 <printint+0x98>
}
 204:	02813083          	ld	ra,40(sp)
 208:	02013403          	ld	s0,32(sp)
 20c:	01813483          	ld	s1,24(sp)
 210:	01013903          	ld	s2,16(sp)
 214:	03010113          	addi	sp,sp,48
 218:	00008067          	ret

000000000000021c <printptr>:

static void
printptr(int fd, uint64 x) {
 21c:	fd010113          	addi	sp,sp,-48
 220:	02113423          	sd	ra,40(sp)
 224:	02813023          	sd	s0,32(sp)
 228:	00913c23          	sd	s1,24(sp)
 22c:	01213823          	sd	s2,16(sp)
 230:	01313423          	sd	s3,8(sp)
 234:	03010413          	addi	s0,sp,48
 238:	00050993          	mv	s3,a0
 23c:	00058493          	mv	s1,a1
  int i;
  putc(fd, '0');
 240:	03000593          	li	a1,48
 244:	00000097          	auipc	ra,0x0
 248:	ed4080e7          	jalr	-300(ra) # 118 <putc>
  putc(fd, 'x');
 24c:	07800593          	li	a1,120
 250:	00098513          	mv	a0,s3
 254:	00000097          	auipc	ra,0x0
 258:	ec4080e7          	jalr	-316(ra) # 118 <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
 25c:	00000913          	li	s2,0
 260:	02c0006f          	j	28c <printptr+0x70>
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
 264:	03c4d713          	srli	a4,s1,0x3c
 268:	00001797          	auipc	a5,0x1
 26c:	a5078793          	addi	a5,a5,-1456 # cb8 <digits>
 270:	00e787b3          	add	a5,a5,a4
 274:	0007c583          	lbu	a1,0(a5)
 278:	00098513          	mv	a0,s3
 27c:	00000097          	auipc	ra,0x0
 280:	e9c080e7          	jalr	-356(ra) # 118 <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
 284:	0019091b          	addiw	s2,s2,1
 288:	00449493          	slli	s1,s1,0x4
 28c:	0009079b          	sext.w	a5,s2
 290:	00f00713          	li	a4,15
 294:	fcf778e3          	bgeu	a4,a5,264 <printptr+0x48>
}
 298:	02813083          	ld	ra,40(sp)
 29c:	02013403          	ld	s0,32(sp)
 2a0:	01813483          	ld	s1,24(sp)
 2a4:	01013903          	ld	s2,16(sp)
 2a8:	00813983          	ld	s3,8(sp)
 2ac:	03010113          	addi	sp,sp,48
 2b0:	00008067          	ret

00000000000002b4 <vprintf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
vprintf(int fd, const char *fmt, va_list ap)
{
 2b4:	fb010113          	addi	sp,sp,-80
 2b8:	04113423          	sd	ra,72(sp)
 2bc:	04813023          	sd	s0,64(sp)
 2c0:	02913c23          	sd	s1,56(sp)
 2c4:	03213823          	sd	s2,48(sp)
 2c8:	03313423          	sd	s3,40(sp)
 2cc:	03413023          	sd	s4,32(sp)
 2d0:	01513c23          	sd	s5,24(sp)
 2d4:	05010413          	addi	s0,sp,80
 2d8:	00050a93          	mv	s5,a0
 2dc:	00058a13          	mv	s4,a1
 2e0:	fac43c23          	sd	a2,-72(s0)
  char *s;
  int c, i, state;

  state = 0;
 2e4:	00000993          	li	s3,0
  for(i = 0; fmt[i]; i++){
 2e8:	00000913          	li	s2,0
 2ec:	0240006f          	j	310 <vprintf+0x5c>
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
 2f0:	00048593          	mv	a1,s1
 2f4:	000a8513          	mv	a0,s5
 2f8:	00000097          	auipc	ra,0x0
 2fc:	e20080e7          	jalr	-480(ra) # 118 <putc>
 300:	00c0006f          	j	30c <vprintf+0x58>
      }
    } else if(state == '%'){
 304:	02500793          	li	a5,37
 308:	02f98663          	beq	s3,a5,334 <vprintf+0x80>
  for(i = 0; fmt[i]; i++){
 30c:	0019091b          	addiw	s2,s2,1
 310:	012a07b3          	add	a5,s4,s2
 314:	0007c483          	lbu	s1,0(a5)
 318:	1a048663          	beqz	s1,4c4 <vprintf+0x210>
    c = fmt[i] & 0xff;
 31c:	0004879b          	sext.w	a5,s1
    if(state == 0){
 320:	fe0992e3          	bnez	s3,304 <vprintf+0x50>
      if(c == '%'){
 324:	02500713          	li	a4,37
 328:	fce794e3          	bne	a5,a4,2f0 <vprintf+0x3c>
        state = '%';
 32c:	00078993          	mv	s3,a5
 330:	fddff06f          	j	30c <vprintf+0x58>
      if(c == 'd'){
 334:	14f48863          	beq	s1,a5,484 <vprintf+0x1d0>
 338:	16f4e263          	bltu	s1,a5,49c <vprintf+0x1e8>
 33c:	07800793          	li	a5,120
 340:	1497ee63          	bltu	a5,s1,49c <vprintf+0x1e8>
 344:	06300793          	li	a5,99
 348:	14f4ea63          	bltu	s1,a5,49c <vprintf+0x1e8>
 34c:	f9d4879b          	addiw	a5,s1,-99
 350:	0ff7f693          	zext.b	a3,a5
 354:	01500713          	li	a4,21
 358:	14d76263          	bltu	a4,a3,49c <vprintf+0x1e8>
 35c:	00269793          	slli	a5,a3,0x2
 360:	00001717          	auipc	a4,0x1
 364:	90070713          	addi	a4,a4,-1792 # c60 <timetag+0x84>
 368:	00e787b3          	add	a5,a5,a4
 36c:	0007a783          	lw	a5,0(a5)
 370:	00e787b3          	add	a5,a5,a4
 374:	00078067          	jr	a5
        printint(fd, va_arg(ap, int), 10, 1);
 378:	fb843783          	ld	a5,-72(s0)
 37c:	00878713          	addi	a4,a5,8
 380:	fae43c23          	sd	a4,-72(s0)
 384:	00100693          	li	a3,1
 388:	00a00613          	li	a2,10
 38c:	0007a583          	lw	a1,0(a5)
 390:	000a8513          	mv	a0,s5
 394:	00000097          	auipc	ra,0x0
 398:	db8080e7          	jalr	-584(ra) # 14c <printint>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 39c:	00000993          	li	s3,0
 3a0:	f6dff06f          	j	30c <vprintf+0x58>
        printint(fd, va_arg(ap, uint64), 10, 0);
 3a4:	fb843783          	ld	a5,-72(s0)
 3a8:	00878713          	addi	a4,a5,8
 3ac:	fae43c23          	sd	a4,-72(s0)
 3b0:	00000693          	li	a3,0
 3b4:	00a00613          	li	a2,10
 3b8:	0007a583          	lw	a1,0(a5)
 3bc:	000a8513          	mv	a0,s5
 3c0:	00000097          	auipc	ra,0x0
 3c4:	d8c080e7          	jalr	-628(ra) # 14c <printint>
      state = 0;
 3c8:	00000993          	li	s3,0
 3cc:	f41ff06f          	j	30c <vprintf+0x58>
        printint(fd, va_arg(ap, int), 16, 0);
 3d0:	fb843783          	ld	a5,-72(s0)
 3d4:	00878713          	addi	a4,a5,8
 3d8:	fae43c23          	sd	a4,-72(s0)
 3dc:	00000693          	li	a3,0
 3e0:	01000613          	li	a2,16
 3e4:	0007a583          	lw	a1,0(a5)
 3e8:	000a8513          	mv	a0,s5
 3ec:	00000097          	auipc	ra,0x0
 3f0:	d60080e7          	jalr	-672(ra) # 14c <printint>
      state = 0;
 3f4:	00000993          	li	s3,0
 3f8:	f15ff06f          	j	30c <vprintf+0x58>
        printptr(fd, va_arg(ap, uint64));
 3fc:	fb843783          	ld	a5,-72(s0)
 400:	00878713          	addi	a4,a5,8
 404:	fae43c23          	sd	a4,-72(s0)
 408:	0007b583          	ld	a1,0(a5)
 40c:	000a8513          	mv	a0,s5
 410:	00000097          	auipc	ra,0x0
 414:	e0c080e7          	jalr	-500(ra) # 21c <printptr>
      state = 0;
 418:	00000993          	li	s3,0
 41c:	ef1ff06f          	j	30c <vprintf+0x58>
        s = va_arg(ap, char*);
 420:	fb843783          	ld	a5,-72(s0)
 424:	00878713          	addi	a4,a5,8
 428:	fae43c23          	sd	a4,-72(s0)
 42c:	0007b483          	ld	s1,0(a5)
        if(s == 0)
 430:	02049063          	bnez	s1,450 <vprintf+0x19c>
          s = "(null)";
 434:	00001497          	auipc	s1,0x1
 438:	89c48493          	addi	s1,s1,-1892 # cd0 <digits+0x18>
 43c:	0140006f          	j	450 <vprintf+0x19c>
          putc(fd, *s);
 440:	000a8513          	mv	a0,s5
 444:	00000097          	auipc	ra,0x0
 448:	cd4080e7          	jalr	-812(ra) # 118 <putc>
          s++;
 44c:	00148493          	addi	s1,s1,1
        while(*s != 0){
 450:	0004c583          	lbu	a1,0(s1)
 454:	fe0596e3          	bnez	a1,440 <vprintf+0x18c>
      state = 0;
 458:	00000993          	li	s3,0
 45c:	eb1ff06f          	j	30c <vprintf+0x58>
        putc(fd, va_arg(ap, uint));
 460:	fb843783          	ld	a5,-72(s0)
 464:	00878713          	addi	a4,a5,8
 468:	fae43c23          	sd	a4,-72(s0)
 46c:	0007c583          	lbu	a1,0(a5)
 470:	000a8513          	mv	a0,s5
 474:	00000097          	auipc	ra,0x0
 478:	ca4080e7          	jalr	-860(ra) # 118 <putc>
      state = 0;
 47c:	00000993          	li	s3,0
 480:	e8dff06f          	j	30c <vprintf+0x58>
        putc(fd, c);
 484:	00048593          	mv	a1,s1
 488:	000a8513          	mv	a0,s5
 48c:	00000097          	auipc	ra,0x0
 490:	c8c080e7          	jalr	-884(ra) # 118 <putc>
      state = 0;
 494:	00000993          	li	s3,0
 498:	e75ff06f          	j	30c <vprintf+0x58>
        putc(fd, '%');
 49c:	02500593          	li	a1,37
 4a0:	000a8513          	mv	a0,s5
 4a4:	00000097          	auipc	ra,0x0
 4a8:	c74080e7          	jalr	-908(ra) # 118 <putc>
        putc(fd, c);
 4ac:	00048593          	mv	a1,s1
 4b0:	000a8513          	mv	a0,s5
 4b4:	00000097          	auipc	ra,0x0
 4b8:	c64080e7          	jalr	-924(ra) # 118 <putc>
      state = 0;
 4bc:	00000993          	li	s3,0
 4c0:	e4dff06f          	j	30c <vprintf+0x58>
    }
  }
}
 4c4:	04813083          	ld	ra,72(sp)
 4c8:	04013403          	ld	s0,64(sp)
 4cc:	03813483          	ld	s1,56(sp)
 4d0:	03013903          	ld	s2,48(sp)
 4d4:	02813983          	ld	s3,40(sp)
 4d8:	02013a03          	ld	s4,32(sp)
 4dc:	01813a83          	ld	s5,24(sp)
 4e0:	05010113          	addi	sp,sp,80
 4e4:	00008067          	ret

00000000000004e8 <fprintf>:

void
fprintf(int fd, const char *fmt, ...)
{
 4e8:	fb010113          	addi	sp,sp,-80
 4ec:	00113c23          	sd	ra,24(sp)
 4f0:	00813823          	sd	s0,16(sp)
 4f4:	02010413          	addi	s0,sp,32
 4f8:	00c43023          	sd	a2,0(s0)
 4fc:	00d43423          	sd	a3,8(s0)
 500:	00e43823          	sd	a4,16(s0)
 504:	00f43c23          	sd	a5,24(s0)
 508:	03043023          	sd	a6,32(s0)
 50c:	03143423          	sd	a7,40(s0)
  va_list ap;

  va_start(ap, fmt);
 510:	fe843423          	sd	s0,-24(s0)
  vprintf(fd, fmt, ap);
 514:	00040613          	mv	a2,s0
 518:	00000097          	auipc	ra,0x0
 51c:	d9c080e7          	jalr	-612(ra) # 2b4 <vprintf>
}
 520:	01813083          	ld	ra,24(sp)
 524:	01013403          	ld	s0,16(sp)
 528:	05010113          	addi	sp,sp,80
 52c:	00008067          	ret

0000000000000530 <printf>:

void
printf(const char *fmt, ...)
{
 530:	fa010113          	addi	sp,sp,-96
 534:	00113c23          	sd	ra,24(sp)
 538:	00813823          	sd	s0,16(sp)
 53c:	02010413          	addi	s0,sp,32
 540:	00b43423          	sd	a1,8(s0)
 544:	00c43823          	sd	a2,16(s0)
 548:	00d43c23          	sd	a3,24(s0)
 54c:	02e43023          	sd	a4,32(s0)
 550:	02f43423          	sd	a5,40(s0)
 554:	03043823          	sd	a6,48(s0)
 558:	03143c23          	sd	a7,56(s0)
  va_list ap;

  va_start(ap, fmt);
 55c:	00840613          	addi	a2,s0,8
 560:	fec43423          	sd	a2,-24(s0)
  vprintf(1, fmt, ap);
 564:	00050593          	mv	a1,a0
 568:	00100513          	li	a0,1
 56c:	00000097          	auipc	ra,0x0
 570:	d48080e7          	jalr	-696(ra) # 2b4 <vprintf>
}
 574:	01813083          	ld	ra,24(sp)
 578:	01013403          	ld	s0,16(sp)
 57c:	06010113          	addi	sp,sp,96
 580:	00008067          	ret

0000000000000584 <strcpy>:
#include "fs/fcntl.h"
#include "user.h"

char*
strcpy(char *s, const char *t)
{
 584:	ff010113          	addi	sp,sp,-16
 588:	00813423          	sd	s0,8(sp)
 58c:	01010413          	addi	s0,sp,16
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 590:	00050693          	mv	a3,a0
 594:	0005c703          	lbu	a4,0(a1)
 598:	00158593          	addi	a1,a1,1
 59c:	00e68023          	sb	a4,0(a3)
 5a0:	00168693          	addi	a3,a3,1
 5a4:	fe0718e3          	bnez	a4,594 <strcpy+0x10>
    ;
  return os;
}
 5a8:	00813403          	ld	s0,8(sp)
 5ac:	01010113          	addi	sp,sp,16
 5b0:	00008067          	ret

00000000000005b4 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 5b4:	ff010113          	addi	sp,sp,-16
 5b8:	00813423          	sd	s0,8(sp)
 5bc:	01010413          	addi	s0,sp,16
  while(*p && *p == *q)
 5c0:	00c0006f          	j	5cc <strcmp+0x18>
    p++, q++;
 5c4:	00150513          	addi	a0,a0,1
 5c8:	00158593          	addi	a1,a1,1
  while(*p && *p == *q)
 5cc:	00054783          	lbu	a5,0(a0)
 5d0:	00078663          	beqz	a5,5dc <strcmp+0x28>
 5d4:	0005c703          	lbu	a4,0(a1)
 5d8:	fee786e3          	beq	a5,a4,5c4 <strcmp+0x10>
  return (uchar)*p - (uchar)*q;
 5dc:	0005c503          	lbu	a0,0(a1)
}
 5e0:	40a7853b          	subw	a0,a5,a0
 5e4:	00813403          	ld	s0,8(sp)
 5e8:	01010113          	addi	sp,sp,16
 5ec:	00008067          	ret

00000000000005f0 <strlen>:

uint
strlen(const char *s)
{
 5f0:	ff010113          	addi	sp,sp,-16
 5f4:	00813423          	sd	s0,8(sp)
 5f8:	01010413          	addi	s0,sp,16
 5fc:	00050713          	mv	a4,a0
  int n;

  for(n = 0; s[n]; n++)
 600:	00000513          	li	a0,0
 604:	0080006f          	j	60c <strlen+0x1c>
 608:	0015051b          	addiw	a0,a0,1
 60c:	00a707b3          	add	a5,a4,a0
 610:	0007c783          	lbu	a5,0(a5)
 614:	fe079ae3          	bnez	a5,608 <strlen+0x18>
    ;
  return n;
}
 618:	00813403          	ld	s0,8(sp)
 61c:	01010113          	addi	sp,sp,16
 620:	00008067          	ret

0000000000000624 <memset>:

void*
memset(void *dst, int c, uint n)
{
 624:	ff010113          	addi	sp,sp,-16
 628:	00813423          	sd	s0,8(sp)
 62c:	01010413          	addi	s0,sp,16
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
 630:	00000793          	li	a5,0
 634:	0100006f          	j	644 <memset+0x20>
    cdst[i] = c;
 638:	00f50733          	add	a4,a0,a5
 63c:	00b70023          	sb	a1,0(a4)
  for(i = 0; i < n; i++){
 640:	0017879b          	addiw	a5,a5,1
 644:	0007871b          	sext.w	a4,a5
 648:	fec768e3          	bltu	a4,a2,638 <memset+0x14>
  }
  return dst;
}
 64c:	00813403          	ld	s0,8(sp)
 650:	01010113          	addi	sp,sp,16
 654:	00008067          	ret

0000000000000658 <strchr>:

char*
strchr(const char *s, char c)
{
 658:	ff010113          	addi	sp,sp,-16
 65c:	00813423          	sd	s0,8(sp)
 660:	01010413          	addi	s0,sp,16
  for(; *s; s++)
 664:	0080006f          	j	66c <strchr+0x14>
 668:	00150513          	addi	a0,a0,1
 66c:	00054783          	lbu	a5,0(a0)
 670:	00078663          	beqz	a5,67c <strchr+0x24>
    if(*s == c)
 674:	feb79ae3          	bne	a5,a1,668 <strchr+0x10>
 678:	0080006f          	j	680 <strchr+0x28>
      return (char*)s;
  return 0;
 67c:	00000513          	li	a0,0
}
 680:	00813403          	ld	s0,8(sp)
 684:	01010113          	addi	sp,sp,16
 688:	00008067          	ret

000000000000068c <gets>:

char*
gets(char *buf, int max)
{
 68c:	fc010113          	addi	sp,sp,-64
 690:	02113c23          	sd	ra,56(sp)
 694:	02813823          	sd	s0,48(sp)
 698:	02913423          	sd	s1,40(sp)
 69c:	03213023          	sd	s2,32(sp)
 6a0:	01313c23          	sd	s3,24(sp)
 6a4:	01413823          	sd	s4,16(sp)
 6a8:	04010413          	addi	s0,sp,64
 6ac:	00050993          	mv	s3,a0
 6b0:	00058a13          	mv	s4,a1
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 6b4:	00000913          	li	s2,0
 6b8:	00090493          	mv	s1,s2
 6bc:	0019091b          	addiw	s2,s2,1
 6c0:	05495263          	bge	s2,s4,704 <gets+0x78>
    cc = read(0, &c, 1);
 6c4:	00100613          	li	a2,1
 6c8:	fcf40593          	addi	a1,s0,-49
 6cc:	00000513          	li	a0,0
 6d0:	00000097          	auipc	ra,0x0
 6d4:	434080e7          	jalr	1076(ra) # b04 <read>
    if(cc < 1)
 6d8:	02a05663          	blez	a0,704 <gets+0x78>
      break;
    buf[i++] = c;
 6dc:	009984b3          	add	s1,s3,s1
 6e0:	fcf44783          	lbu	a5,-49(s0)
 6e4:	00f48023          	sb	a5,0(s1)
    if(c == '\n' || c == '\r')
 6e8:	00a00713          	li	a4,10
 6ec:	00e78a63          	beq	a5,a4,700 <gets+0x74>
 6f0:	00d00713          	li	a4,13
 6f4:	fce792e3          	bne	a5,a4,6b8 <gets+0x2c>
    buf[i++] = c;
 6f8:	00090493          	mv	s1,s2
 6fc:	0080006f          	j	704 <gets+0x78>
 700:	00090493          	mv	s1,s2
      break;
  }
  buf[i] = '\0';
 704:	009984b3          	add	s1,s3,s1
 708:	00048023          	sb	zero,0(s1)
  return buf;
}
 70c:	00098513          	mv	a0,s3
 710:	03813083          	ld	ra,56(sp)
 714:	03013403          	ld	s0,48(sp)
 718:	02813483          	ld	s1,40(sp)
 71c:	02013903          	ld	s2,32(sp)
 720:	01813983          	ld	s3,24(sp)
 724:	01013a03          	ld	s4,16(sp)
 728:	04010113          	addi	sp,sp,64
 72c:	00008067          	ret

0000000000000730 <stat>:

int
stat(const char *n, struct stat *st)
{
 730:	fe010113          	addi	sp,sp,-32
 734:	00113c23          	sd	ra,24(sp)
 738:	00813823          	sd	s0,16(sp)
 73c:	00913423          	sd	s1,8(sp)
 740:	01213023          	sd	s2,0(sp)
 744:	02010413          	addi	s0,sp,32
 748:	00058913          	mv	s2,a1
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 74c:	00000593          	li	a1,0
 750:	00000097          	auipc	ra,0x0
 754:	42c080e7          	jalr	1068(ra) # b7c <open>
  if(fd < 0)
 758:	04054063          	bltz	a0,798 <stat+0x68>
 75c:	00050493          	mv	s1,a0
    return -1;
  r = fstat(fd, st);
 760:	00090593          	mv	a1,s2
 764:	00000097          	auipc	ra,0x0
 768:	3c4080e7          	jalr	964(ra) # b28 <fstat>
 76c:	00050913          	mv	s2,a0
  close(fd);
 770:	00048513          	mv	a0,s1
 774:	00000097          	auipc	ra,0x0
 778:	450080e7          	jalr	1104(ra) # bc4 <close>
  return r;
}
 77c:	00090513          	mv	a0,s2
 780:	01813083          	ld	ra,24(sp)
 784:	01013403          	ld	s0,16(sp)
 788:	00813483          	ld	s1,8(sp)
 78c:	00013903          	ld	s2,0(sp)
 790:	02010113          	addi	sp,sp,32
 794:	00008067          	ret
    return -1;
 798:	fff00913          	li	s2,-1
 79c:	fe1ff06f          	j	77c <stat+0x4c>

00000000000007a0 <atoi>:

int
atoi(const char *s)
{
 7a0:	ff010113          	addi	sp,sp,-16
 7a4:	00813423          	sd	s0,8(sp)
 7a8:	01010413          	addi	s0,sp,16
 7ac:	00050713          	mv	a4,a0
  int n;

  n = 0;
 7b0:	00000513          	li	a0,0
  while('0' <= *s && *s <= '9')
 7b4:	01c0006f          	j	7d0 <atoi+0x30>
    n = n*10 + *s++ - '0';
 7b8:	0025179b          	slliw	a5,a0,0x2
 7bc:	00a787bb          	addw	a5,a5,a0
 7c0:	0017979b          	slliw	a5,a5,0x1
 7c4:	00170713          	addi	a4,a4,1
 7c8:	00d787bb          	addw	a5,a5,a3
 7cc:	fd07851b          	addiw	a0,a5,-48
  while('0' <= *s && *s <= '9')
 7d0:	00074683          	lbu	a3,0(a4)
 7d4:	fd06879b          	addiw	a5,a3,-48
 7d8:	0ff7f793          	zext.b	a5,a5
 7dc:	00900613          	li	a2,9
 7e0:	fcf67ce3          	bgeu	a2,a5,7b8 <atoi+0x18>
  return n;
}
 7e4:	00813403          	ld	s0,8(sp)
 7e8:	01010113          	addi	sp,sp,16
 7ec:	00008067          	ret

00000000000007f0 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 7f0:	ff010113          	addi	sp,sp,-16
 7f4:	00813423          	sd	s0,8(sp)
 7f8:	01010413          	addi	s0,sp,16
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
 7fc:	02b56c63          	bltu	a0,a1,834 <memmove+0x44>
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
 800:	00c507b3          	add	a5,a0,a2
    src += n;
 804:	00c585b3          	add	a1,a1,a2
    while(n-- > 0)
 808:	0480006f          	j	850 <memmove+0x60>
      *dst++ = *src++;
 80c:	0005c683          	lbu	a3,0(a1)
 810:	00d78023          	sb	a3,0(a5)
 814:	00158593          	addi	a1,a1,1
 818:	00178793          	addi	a5,a5,1
    while(n-- > 0)
 81c:	00070613          	mv	a2,a4
 820:	fff6071b          	addiw	a4,a2,-1
 824:	fec044e3          	bgtz	a2,80c <memmove+0x1c>
      *--dst = *--src;
  }
  return vdst;
}
 828:	00813403          	ld	s0,8(sp)
 82c:	01010113          	addi	sp,sp,16
 830:	00008067          	ret
  dst = vdst;
 834:	00050793          	mv	a5,a0
 838:	fe9ff06f          	j	820 <memmove+0x30>
      *--dst = *--src;
 83c:	fff58593          	addi	a1,a1,-1
 840:	fff78793          	addi	a5,a5,-1
 844:	0005c683          	lbu	a3,0(a1)
 848:	00d78023          	sb	a3,0(a5)
    while(n-- > 0)
 84c:	00070613          	mv	a2,a4
 850:	fff6071b          	addiw	a4,a2,-1
 854:	fec044e3          	bgtz	a2,83c <memmove+0x4c>
 858:	fd1ff06f          	j	828 <memmove+0x38>

000000000000085c <memcmp>:

int
memcmp(const void *s1, const void *s2, uint n)
{
 85c:	ff010113          	addi	sp,sp,-16
 860:	00813423          	sd	s0,8(sp)
 864:	01010413          	addi	s0,sp,16
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
 868:	0100006f          	j	878 <memcmp+0x1c>
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
 86c:	00150513          	addi	a0,a0,1
    p2++;
 870:	00158593          	addi	a1,a1,1
  while (n-- > 0) {
 874:	00068613          	mv	a2,a3
 878:	fff6069b          	addiw	a3,a2,-1
 87c:	00060c63          	beqz	a2,894 <memcmp+0x38>
    if (*p1 != *p2) {
 880:	00054783          	lbu	a5,0(a0)
 884:	0005c703          	lbu	a4,0(a1)
 888:	fee782e3          	beq	a5,a4,86c <memcmp+0x10>
      return *p1 - *p2;
 88c:	40e7853b          	subw	a0,a5,a4
 890:	0080006f          	j	898 <memcmp+0x3c>
  }
  return 0;
 894:	00000513          	li	a0,0
}
 898:	00813403          	ld	s0,8(sp)
 89c:	01010113          	addi	sp,sp,16
 8a0:	00008067          	ret

00000000000008a4 <memcpy>:

void *
memcpy(void *dst, const void *src, uint n)
{
 8a4:	ff010113          	addi	sp,sp,-16
 8a8:	00113423          	sd	ra,8(sp)
 8ac:	00813023          	sd	s0,0(sp)
 8b0:	01010413          	addi	s0,sp,16
  return memmove(dst, src, n);
 8b4:	00000097          	auipc	ra,0x0
 8b8:	f3c080e7          	jalr	-196(ra) # 7f0 <memmove>
}
 8bc:	00813083          	ld	ra,8(sp)
 8c0:	00013403          	ld	s0,0(sp)
 8c4:	01010113          	addi	sp,sp,16
 8c8:	00008067          	ret

00000000000008cc <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 8cc:	ff010113          	addi	sp,sp,-16
 8d0:	00813423          	sd	s0,8(sp)
 8d4:	01010413          	addi	s0,sp,16
  Header *bp, *p;

  bp = (Header*)ap - 1;
 8d8:	ff050693          	addi	a3,a0,-16
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8dc:	00000797          	auipc	a5,0x0
 8e0:	40c7b783          	ld	a5,1036(a5) # ce8 <freep>
 8e4:	0080006f          	j	8ec <free+0x20>
 8e8:	00070793          	mv	a5,a4
 8ec:	00d7f663          	bgeu	a5,a3,8f8 <free+0x2c>
 8f0:	0007b703          	ld	a4,0(a5)
 8f4:	00e6ee63          	bltu	a3,a4,910 <free+0x44>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 8f8:	0007b703          	ld	a4,0(a5)
 8fc:	fee7e6e3          	bltu	a5,a4,8e8 <free+0x1c>
 900:	00d7e863          	bltu	a5,a3,910 <free+0x44>
 904:	00e6e663          	bltu	a3,a4,910 <free+0x44>
 908:	00070793          	mv	a5,a4
 90c:	fe1ff06f          	j	8ec <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 910:	ff852583          	lw	a1,-8(a0)
 914:	02059613          	slli	a2,a1,0x20
 918:	01c65713          	srli	a4,a2,0x1c
 91c:	00e68733          	add	a4,a3,a4
 920:	0007b603          	ld	a2,0(a5)
 924:	02c70a63          	beq	a4,a2,958 <free+0x8c>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 928:	fec53823          	sd	a2,-16(a0)
  if(p + p->s.size == bp){
 92c:	0087a603          	lw	a2,8(a5)
 930:	02061593          	slli	a1,a2,0x20
 934:	01c5d713          	srli	a4,a1,0x1c
 938:	00e78733          	add	a4,a5,a4
 93c:	02d70c63          	beq	a4,a3,974 <free+0xa8>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 940:	00d7b023          	sd	a3,0(a5)
  freep = p;
 944:	00000717          	auipc	a4,0x0
 948:	3af73223          	sd	a5,932(a4) # ce8 <freep>
}
 94c:	00813403          	ld	s0,8(sp)
 950:	01010113          	addi	sp,sp,16
 954:	00008067          	ret
    bp->s.size += p->s.ptr->s.size;
 958:	00862703          	lw	a4,8(a2)
 95c:	00e585bb          	addw	a1,a1,a4
 960:	feb52c23          	sw	a1,-8(a0)
    bp->s.ptr = p->s.ptr->s.ptr;
 964:	0007b703          	ld	a4,0(a5)
 968:	00073703          	ld	a4,0(a4)
 96c:	fee53823          	sd	a4,-16(a0)
 970:	fbdff06f          	j	92c <free+0x60>
    p->s.size += bp->s.size;
 974:	ff852703          	lw	a4,-8(a0)
 978:	00e6063b          	addw	a2,a2,a4
 97c:	00c7a423          	sw	a2,8(a5)
    p->s.ptr = bp->s.ptr;
 980:	ff053703          	ld	a4,-16(a0)
 984:	00e7b023          	sd	a4,0(a5)
 988:	fbdff06f          	j	944 <free+0x78>

000000000000098c <morecore>:

static Header*
morecore(uint nu)
{
 98c:	fe010113          	addi	sp,sp,-32
 990:	00113c23          	sd	ra,24(sp)
 994:	00813823          	sd	s0,16(sp)
 998:	00913423          	sd	s1,8(sp)
 99c:	02010413          	addi	s0,sp,32
 9a0:	00050493          	mv	s1,a0
  char *p;
  Header *hp;

  if(nu < 4096)
 9a4:	000017b7          	lui	a5,0x1
 9a8:	00f57463          	bgeu	a0,a5,9b0 <morecore+0x24>
    nu = 4096;
 9ac:	000014b7          	lui	s1,0x1
  p = sbrk(nu * sizeof(Header));
 9b0:	0044951b          	slliw	a0,s1,0x4
 9b4:	00000097          	auipc	ra,0x0
 9b8:	1a4080e7          	jalr	420(ra) # b58 <sbrk>
  if(p == (char*)-1)
 9bc:	fff00793          	li	a5,-1
 9c0:	02f50863          	beq	a0,a5,9f0 <morecore+0x64>
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
 9c4:	00952423          	sw	s1,8(a0)
  free((void*)(hp + 1));
 9c8:	01050513          	addi	a0,a0,16
 9cc:	00000097          	auipc	ra,0x0
 9d0:	f00080e7          	jalr	-256(ra) # 8cc <free>
  return freep;
 9d4:	00000517          	auipc	a0,0x0
 9d8:	31453503          	ld	a0,788(a0) # ce8 <freep>
}
 9dc:	01813083          	ld	ra,24(sp)
 9e0:	01013403          	ld	s0,16(sp)
 9e4:	00813483          	ld	s1,8(sp)
 9e8:	02010113          	addi	sp,sp,32
 9ec:	00008067          	ret
    return 0;
 9f0:	00000513          	li	a0,0
 9f4:	fe9ff06f          	j	9dc <morecore+0x50>

00000000000009f8 <malloc>:

void*
malloc(uint nbytes)
{
 9f8:	fe010113          	addi	sp,sp,-32
 9fc:	00113c23          	sd	ra,24(sp)
 a00:	00813823          	sd	s0,16(sp)
 a04:	00913423          	sd	s1,8(sp)
 a08:	01213023          	sd	s2,0(sp)
 a0c:	02010413          	addi	s0,sp,32
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 a10:	02051493          	slli	s1,a0,0x20
 a14:	0204d493          	srli	s1,s1,0x20
 a18:	00f48493          	addi	s1,s1,15 # 100f <__BSS_END__+0x30f>
 a1c:	0044d493          	srli	s1,s1,0x4
 a20:	0014891b          	addiw	s2,s1,1
 a24:	00148493          	addi	s1,s1,1
  if((prevp = freep) == 0){
 a28:	00000717          	auipc	a4,0x0
 a2c:	2c073703          	ld	a4,704(a4) # ce8 <freep>
 a30:	00070663          	beqz	a4,a3c <malloc+0x44>
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 a34:	00073503          	ld	a0,0(a4)
 a38:	0740006f          	j	aac <malloc+0xb4>
    base.s.ptr = freep = prevp = &base;
 a3c:	00000717          	auipc	a4,0x0
 a40:	2b470713          	addi	a4,a4,692 # cf0 <base>
 a44:	00000797          	auipc	a5,0x0
 a48:	2ae7b223          	sd	a4,676(a5) # ce8 <freep>
 a4c:	00e73023          	sd	a4,0(a4)
    base.s.size = 0;
 a50:	00072423          	sw	zero,8(a4)
 a54:	fe1ff06f          	j	a34 <malloc+0x3c>
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
 a58:	04978063          	beq	a5,s1,a98 <malloc+0xa0>
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
 a5c:	412787bb          	subw	a5,a5,s2
 a60:	00f52423          	sw	a5,8(a0)
        p += p->s.size;
 a64:	02079693          	slli	a3,a5,0x20
 a68:	01c6d793          	srli	a5,a3,0x1c
 a6c:	00f50533          	add	a0,a0,a5
        p->s.size = nunits;
 a70:	01252423          	sw	s2,8(a0)
      }
      freep = prevp;
 a74:	00000797          	auipc	a5,0x0
 a78:	26e7ba23          	sd	a4,628(a5) # ce8 <freep>
      return (void*)(p + 1);
 a7c:	01050513          	addi	a0,a0,16
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
 a80:	01813083          	ld	ra,24(sp)
 a84:	01013403          	ld	s0,16(sp)
 a88:	00813483          	ld	s1,8(sp)
 a8c:	00013903          	ld	s2,0(sp)
 a90:	02010113          	addi	sp,sp,32
 a94:	00008067          	ret
        prevp->s.ptr = p->s.ptr;
 a98:	00053783          	ld	a5,0(a0)
 a9c:	00f73023          	sd	a5,0(a4)
 aa0:	fd5ff06f          	j	a74 <malloc+0x7c>
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 aa4:	00050713          	mv	a4,a0
 aa8:	00053503          	ld	a0,0(a0)
    if(p->s.size >= nunits){
 aac:	00852783          	lw	a5,8(a0)
 ab0:	fa97f4e3          	bgeu	a5,s1,a58 <malloc+0x60>
    if(p == freep)
 ab4:	00000797          	auipc	a5,0x0
 ab8:	2347b783          	ld	a5,564(a5) # ce8 <freep>
 abc:	fea794e3          	bne	a5,a0,aa4 <malloc+0xac>
      if((p = morecore(nunits)) == 0)
 ac0:	00048513          	mv	a0,s1
 ac4:	00000097          	auipc	ra,0x0
 ac8:	ec8080e7          	jalr	-312(ra) # 98c <morecore>
 acc:	fc051ce3          	bnez	a0,aa4 <malloc+0xac>
 ad0:	fb1ff06f          	j	a80 <malloc+0x88>

0000000000000ad4 <fork>:

#ifndef __SYS_CALL
#define __SYS_CALL
#endif

__SYS_CALL(1, fork, sys_fork)
 ad4:	00100893          	li	a7,1
 ad8:	00000073          	ecall
 adc:	00008067          	ret

0000000000000ae0 <exit>:
__SYS_CALL(2, exit, sys_exit)
 ae0:	00200893          	li	a7,2
 ae4:	00000073          	ecall
 ae8:	00008067          	ret

0000000000000aec <wait>:
__SYS_CALL(3, wait, sys_wait)
 aec:	00300893          	li	a7,3
 af0:	00000073          	ecall
 af4:	00008067          	ret

0000000000000af8 <pipe>:
__SYS_CALL(4, pipe, sys_pipe)
 af8:	00400893          	li	a7,4
 afc:	00000073          	ecall
 b00:	00008067          	ret

0000000000000b04 <read>:
__SYS_CALL(5, read, sys_read)
 b04:	00500893          	li	a7,5
 b08:	00000073          	ecall
 b0c:	00008067          	ret

0000000000000b10 <kill>:
__SYS_CALL(6, kill, sys_kill)
 b10:	00600893          	li	a7,6
 b14:	00000073          	ecall
 b18:	00008067          	ret

0000000000000b1c <exec>:
__SYS_CALL(7, exec, sys_exec)
 b1c:	00700893          	li	a7,7
 b20:	00000073          	ecall
 b24:	00008067          	ret

0000000000000b28 <fstat>:
__SYS_CALL(8, fstat, sys_fstat)
 b28:	00800893          	li	a7,8
 b2c:	00000073          	ecall
 b30:	00008067          	ret

0000000000000b34 <chdir>:
__SYS_CALL(9, chdir, sys_chdir)
 b34:	00900893          	li	a7,9
 b38:	00000073          	ecall
 b3c:	00008067          	ret

0000000000000b40 <dup>:
__SYS_CALL(10, dup, sys_dup)
 b40:	00a00893          	li	a7,10
 b44:	00000073          	ecall
 b48:	00008067          	ret

0000000000000b4c <getpid>:
__SYS_CALL(11, getpid, sys_getpid)
 b4c:	00b00893          	li	a7,11
 b50:	00000073          	ecall
 b54:	00008067          	ret

0000000000000b58 <sbrk>:
__SYS_CALL(12, sbrk, sys_sbrk)
 b58:	00c00893          	li	a7,12
 b5c:	00000073          	ecall
 b60:	00008067          	ret

0000000000000b64 <sleep>:
__SYS_CALL(13, sleep, sys_sleep)
 b64:	00d00893          	li	a7,13
 b68:	00000073          	ecall
 b6c:	00008067          	ret

0000000000000b70 <uptime>:
__SYS_CALL(14, uptime, sys_uptime)
 b70:	00e00893          	li	a7,14
 b74:	00000073          	ecall
 b78:	00008067          	ret

0000000000000b7c <open>:
__SYS_CALL(15, open, sys_open)
 b7c:	00f00893          	li	a7,15
 b80:	00000073          	ecall
 b84:	00008067          	ret

0000000000000b88 <write>:
__SYS_CALL(16, write, sys_write)
 b88:	01000893          	li	a7,16
 b8c:	00000073          	ecall
 b90:	00008067          	ret

0000000000000b94 <mknod>:
__SYS_CALL(17, mknod, sys_mknod)
 b94:	01100893          	li	a7,17
 b98:	00000073          	ecall
 b9c:	00008067          	ret

0000000000000ba0 <unlink>:
__SYS_CALL(18, unlink, sys_unlink)
 ba0:	01200893          	li	a7,18
 ba4:	00000073          	ecall
 ba8:	00008067          	ret

0000000000000bac <link>:
__SYS_CALL(19, link, sys_link)
 bac:	01300893          	li	a7,19
 bb0:	00000073          	ecall
 bb4:	00008067          	ret

0000000000000bb8 <mkdir>:
__SYS_CALL(20, mkdir, sys_mkdir)
 bb8:	01400893          	li	a7,20
 bbc:	00000073          	ecall
 bc0:	00008067          	ret

0000000000000bc4 <close>:
__SYS_CALL(21, close, sys_close)
 bc4:	01500893          	li	a7,21
 bc8:	00000073          	ecall
 bcc:	00008067          	ret

0000000000000bd0 <ktest>:
__SYS_CALL(88, ktest, sys_ktest)
 bd0:	05800893          	li	a7,88
 bd4:	00000073          	ecall
 bd8:	00008067          	ret

0000000000000bdc <timetag>:
 bdc:	05900893          	li	a7,89
 be0:	00000073          	ecall
 be4:	00008067          	ret
