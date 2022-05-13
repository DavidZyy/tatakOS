
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

  

  // printf(red("init run!\n"));

  if(open("console", O_RDWR) < 0){
  14:	00200593          	li	a1,2
  18:	00001517          	auipc	a0,0x1
  1c:	be850513          	addi	a0,a0,-1048 # c00 <timetag+0x10>
  20:	00001097          	auipc	ra,0x1
  24:	b70080e7          	jalr	-1168(ra) # b90 <open>
  28:	06054863          	bltz	a0,98 <main+0x98>
    mknod("console", CONSOLE, 0);
    open("console", O_RDWR);
  }
  dup(0);  // stdout
  2c:	00000513          	li	a0,0
  30:	00001097          	auipc	ra,0x1
  34:	b24080e7          	jalr	-1244(ra) # b54 <dup>
  dup(0);  // stderr
  38:	00000513          	li	a0,0
  3c:	00001097          	auipc	ra,0x1
  40:	b18080e7          	jalr	-1256(ra) # b54 <dup>

  for(;;){
    printf(yellow("init: starting sh\n"));
  44:	00001517          	auipc	a0,0x1
  48:	bc450513          	addi	a0,a0,-1084 # c08 <timetag+0x18>
  4c:	00000097          	auipc	ra,0x0
  50:	4f8080e7          	jalr	1272(ra) # 544 <printf>
    pid = fork();
  54:	00001097          	auipc	ra,0x1
  58:	a94080e7          	jalr	-1388(ra) # ae8 <fork>
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
  70:	a94080e7          	jalr	-1388(ra) # b00 <wait>
      if(wpid == pid){
  74:	0aa48263          	beq	s1,a0,118 <main+0x118>
        // the shell exited; restart it.
        printf(green("shell exit!!\n"));
        for(;;);
        break;
      } else if(wpid < 0){
  78:	fe0558e3          	bgez	a0,68 <main+0x68>
        printf("init: wait returned an error\n");
  7c:	00001517          	auipc	a0,0x1
  80:	bfc50513          	addi	a0,a0,-1028 # c78 <timetag+0x88>
  84:	00000097          	auipc	ra,0x0
  88:	4c0080e7          	jalr	1216(ra) # 544 <printf>
        exit(1);
  8c:	00100513          	li	a0,1
  90:	00001097          	auipc	ra,0x1
  94:	a64080e7          	jalr	-1436(ra) # af4 <exit>
    mknod("console", CONSOLE, 0);
  98:	00000613          	li	a2,0
  9c:	00100593          	li	a1,1
  a0:	00001517          	auipc	a0,0x1
  a4:	b6050513          	addi	a0,a0,-1184 # c00 <timetag+0x10>
  a8:	00001097          	auipc	ra,0x1
  ac:	b00080e7          	jalr	-1280(ra) # ba8 <mknod>
    open("console", O_RDWR);
  b0:	00200593          	li	a1,2
  b4:	00001517          	auipc	a0,0x1
  b8:	b4c50513          	addi	a0,a0,-1204 # c00 <timetag+0x10>
  bc:	00001097          	auipc	ra,0x1
  c0:	ad4080e7          	jalr	-1324(ra) # b90 <open>
  c4:	f69ff06f          	j	2c <main+0x2c>
      printf("init: fork failed\n");
  c8:	00001517          	auipc	a0,0x1
  cc:	b6050513          	addi	a0,a0,-1184 # c28 <timetag+0x38>
  d0:	00000097          	auipc	ra,0x0
  d4:	474080e7          	jalr	1140(ra) # 544 <printf>
      exit(1);
  d8:	00100513          	li	a0,1
  dc:	00001097          	auipc	ra,0x1
  e0:	a18080e7          	jalr	-1512(ra) # af4 <exit>
      exec("sh", argv);
  e4:	00001597          	auipc	a1,0x1
  e8:	c2c58593          	addi	a1,a1,-980 # d10 <argv>
  ec:	00001517          	auipc	a0,0x1
  f0:	b5450513          	addi	a0,a0,-1196 # c40 <timetag+0x50>
  f4:	00001097          	auipc	ra,0x1
  f8:	a3c080e7          	jalr	-1476(ra) # b30 <exec>
      printf("init: exec sh failed\n");
  fc:	00001517          	auipc	a0,0x1
 100:	b4c50513          	addi	a0,a0,-1204 # c48 <timetag+0x58>
 104:	00000097          	auipc	ra,0x0
 108:	440080e7          	jalr	1088(ra) # 544 <printf>
      exit(1);
 10c:	00100513          	li	a0,1
 110:	00001097          	auipc	ra,0x1
 114:	9e4080e7          	jalr	-1564(ra) # af4 <exit>
        printf(green("shell exit!!\n"));
 118:	00001517          	auipc	a0,0x1
 11c:	b4850513          	addi	a0,a0,-1208 # c60 <timetag+0x70>
 120:	00000097          	auipc	ra,0x0
 124:	424080e7          	jalr	1060(ra) # 544 <printf>
        for(;;);
 128:	0000006f          	j	128 <main+0x128>

000000000000012c <putc>:

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
 12c:	fe010113          	addi	sp,sp,-32
 130:	00113c23          	sd	ra,24(sp)
 134:	00813823          	sd	s0,16(sp)
 138:	02010413          	addi	s0,sp,32
 13c:	feb407a3          	sb	a1,-17(s0)
  write(fd, &c, 1);
 140:	00100613          	li	a2,1
 144:	fef40593          	addi	a1,s0,-17
 148:	00001097          	auipc	ra,0x1
 14c:	a54080e7          	jalr	-1452(ra) # b9c <write>
}
 150:	01813083          	ld	ra,24(sp)
 154:	01013403          	ld	s0,16(sp)
 158:	02010113          	addi	sp,sp,32
 15c:	00008067          	ret

0000000000000160 <printint>:

static void
printint(int fd, int xx, int base, int sgn)
{
 160:	fd010113          	addi	sp,sp,-48
 164:	02113423          	sd	ra,40(sp)
 168:	02813023          	sd	s0,32(sp)
 16c:	00913c23          	sd	s1,24(sp)
 170:	01213823          	sd	s2,16(sp)
 174:	03010413          	addi	s0,sp,48
 178:	00050913          	mv	s2,a0
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
 17c:	00068463          	beqz	a3,184 <printint+0x24>
 180:	0605c663          	bltz	a1,1ec <printint+0x8c>
    neg = 1;
    x = -xx;
  } else {
    x = xx;
 184:	0005859b          	sext.w	a1,a1
  neg = 0;
 188:	00000813          	li	a6,0
  }

  i = 0;
 18c:	00000493          	li	s1,0
  do{
    buf[i++] = digits[x % base];
 190:	0006079b          	sext.w	a5,a2
 194:	02c5f6bb          	remuw	a3,a1,a2
 198:	00048513          	mv	a0,s1
 19c:	0014849b          	addiw	s1,s1,1
 1a0:	02069693          	slli	a3,a3,0x20
 1a4:	0206d693          	srli	a3,a3,0x20
 1a8:	00001717          	auipc	a4,0x1
 1ac:	b4870713          	addi	a4,a4,-1208 # cf0 <digits>
 1b0:	00d70733          	add	a4,a4,a3
 1b4:	00074683          	lbu	a3,0(a4)
 1b8:	fe050713          	addi	a4,a0,-32
 1bc:	00870733          	add	a4,a4,s0
 1c0:	fed70823          	sb	a3,-16(a4)
  }while((x /= base) != 0);
 1c4:	0005871b          	sext.w	a4,a1
 1c8:	02c5d5bb          	divuw	a1,a1,a2
 1cc:	fcf772e3          	bgeu	a4,a5,190 <printint+0x30>
  if(neg)
 1d0:	04080063          	beqz	a6,210 <printint+0xb0>
    buf[i++] = '-';
 1d4:	fe048793          	addi	a5,s1,-32
 1d8:	008784b3          	add	s1,a5,s0
 1dc:	02d00793          	li	a5,45
 1e0:	fef48823          	sb	a5,-16(s1)
 1e4:	0025049b          	addiw	s1,a0,2
 1e8:	0280006f          	j	210 <printint+0xb0>
    x = -xx;
 1ec:	40b005bb          	negw	a1,a1
    neg = 1;
 1f0:	00100813          	li	a6,1
    x = -xx;
 1f4:	f99ff06f          	j	18c <printint+0x2c>

  while(--i >= 0)
    putc(fd, buf[i]);
 1f8:	fe048793          	addi	a5,s1,-32
 1fc:	008787b3          	add	a5,a5,s0
 200:	ff07c583          	lbu	a1,-16(a5)
 204:	00090513          	mv	a0,s2
 208:	00000097          	auipc	ra,0x0
 20c:	f24080e7          	jalr	-220(ra) # 12c <putc>
  while(--i >= 0)
 210:	fff4849b          	addiw	s1,s1,-1
 214:	fe04d2e3          	bgez	s1,1f8 <printint+0x98>
}
 218:	02813083          	ld	ra,40(sp)
 21c:	02013403          	ld	s0,32(sp)
 220:	01813483          	ld	s1,24(sp)
 224:	01013903          	ld	s2,16(sp)
 228:	03010113          	addi	sp,sp,48
 22c:	00008067          	ret

0000000000000230 <printptr>:

static void
printptr(int fd, uint64 x) {
 230:	fd010113          	addi	sp,sp,-48
 234:	02113423          	sd	ra,40(sp)
 238:	02813023          	sd	s0,32(sp)
 23c:	00913c23          	sd	s1,24(sp)
 240:	01213823          	sd	s2,16(sp)
 244:	01313423          	sd	s3,8(sp)
 248:	03010413          	addi	s0,sp,48
 24c:	00050993          	mv	s3,a0
 250:	00058493          	mv	s1,a1
  int i;
  putc(fd, '0');
 254:	03000593          	li	a1,48
 258:	00000097          	auipc	ra,0x0
 25c:	ed4080e7          	jalr	-300(ra) # 12c <putc>
  putc(fd, 'x');
 260:	07800593          	li	a1,120
 264:	00098513          	mv	a0,s3
 268:	00000097          	auipc	ra,0x0
 26c:	ec4080e7          	jalr	-316(ra) # 12c <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
 270:	00000913          	li	s2,0
 274:	02c0006f          	j	2a0 <printptr+0x70>
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
 278:	03c4d713          	srli	a4,s1,0x3c
 27c:	00001797          	auipc	a5,0x1
 280:	a7478793          	addi	a5,a5,-1420 # cf0 <digits>
 284:	00e787b3          	add	a5,a5,a4
 288:	0007c583          	lbu	a1,0(a5)
 28c:	00098513          	mv	a0,s3
 290:	00000097          	auipc	ra,0x0
 294:	e9c080e7          	jalr	-356(ra) # 12c <putc>
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
 298:	0019091b          	addiw	s2,s2,1
 29c:	00449493          	slli	s1,s1,0x4
 2a0:	0009079b          	sext.w	a5,s2
 2a4:	00f00713          	li	a4,15
 2a8:	fcf778e3          	bgeu	a4,a5,278 <printptr+0x48>
}
 2ac:	02813083          	ld	ra,40(sp)
 2b0:	02013403          	ld	s0,32(sp)
 2b4:	01813483          	ld	s1,24(sp)
 2b8:	01013903          	ld	s2,16(sp)
 2bc:	00813983          	ld	s3,8(sp)
 2c0:	03010113          	addi	sp,sp,48
 2c4:	00008067          	ret

00000000000002c8 <vprintf>:

// Print to the given fd. Only understands %d, %x, %p, %s.
void
vprintf(int fd, const char *fmt, va_list ap)
{
 2c8:	fb010113          	addi	sp,sp,-80
 2cc:	04113423          	sd	ra,72(sp)
 2d0:	04813023          	sd	s0,64(sp)
 2d4:	02913c23          	sd	s1,56(sp)
 2d8:	03213823          	sd	s2,48(sp)
 2dc:	03313423          	sd	s3,40(sp)
 2e0:	03413023          	sd	s4,32(sp)
 2e4:	01513c23          	sd	s5,24(sp)
 2e8:	05010413          	addi	s0,sp,80
 2ec:	00050a93          	mv	s5,a0
 2f0:	00058a13          	mv	s4,a1
 2f4:	fac43c23          	sd	a2,-72(s0)
  char *s;
  int c, i, state;

  state = 0;
 2f8:	00000993          	li	s3,0
  for(i = 0; fmt[i]; i++){
 2fc:	00000913          	li	s2,0
 300:	0240006f          	j	324 <vprintf+0x5c>
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
 304:	00048593          	mv	a1,s1
 308:	000a8513          	mv	a0,s5
 30c:	00000097          	auipc	ra,0x0
 310:	e20080e7          	jalr	-480(ra) # 12c <putc>
 314:	00c0006f          	j	320 <vprintf+0x58>
      }
    } else if(state == '%'){
 318:	02500793          	li	a5,37
 31c:	02f98663          	beq	s3,a5,348 <vprintf+0x80>
  for(i = 0; fmt[i]; i++){
 320:	0019091b          	addiw	s2,s2,1
 324:	012a07b3          	add	a5,s4,s2
 328:	0007c483          	lbu	s1,0(a5)
 32c:	1a048663          	beqz	s1,4d8 <vprintf+0x210>
    c = fmt[i] & 0xff;
 330:	0004879b          	sext.w	a5,s1
    if(state == 0){
 334:	fe0992e3          	bnez	s3,318 <vprintf+0x50>
      if(c == '%'){
 338:	02500713          	li	a4,37
 33c:	fce794e3          	bne	a5,a4,304 <vprintf+0x3c>
        state = '%';
 340:	00078993          	mv	s3,a5
 344:	fddff06f          	j	320 <vprintf+0x58>
      if(c == 'd'){
 348:	14f48863          	beq	s1,a5,498 <vprintf+0x1d0>
 34c:	16f4e263          	bltu	s1,a5,4b0 <vprintf+0x1e8>
 350:	07800793          	li	a5,120
 354:	1497ee63          	bltu	a5,s1,4b0 <vprintf+0x1e8>
 358:	06300793          	li	a5,99
 35c:	14f4ea63          	bltu	s1,a5,4b0 <vprintf+0x1e8>
 360:	f9d4879b          	addiw	a5,s1,-99
 364:	0ff7f693          	zext.b	a3,a5
 368:	01500713          	li	a4,21
 36c:	14d76263          	bltu	a4,a3,4b0 <vprintf+0x1e8>
 370:	00269793          	slli	a5,a3,0x2
 374:	00001717          	auipc	a4,0x1
 378:	92470713          	addi	a4,a4,-1756 # c98 <timetag+0xa8>
 37c:	00e787b3          	add	a5,a5,a4
 380:	0007a783          	lw	a5,0(a5)
 384:	00e787b3          	add	a5,a5,a4
 388:	00078067          	jr	a5
        printint(fd, va_arg(ap, int), 10, 1);
 38c:	fb843783          	ld	a5,-72(s0)
 390:	00878713          	addi	a4,a5,8
 394:	fae43c23          	sd	a4,-72(s0)
 398:	00100693          	li	a3,1
 39c:	00a00613          	li	a2,10
 3a0:	0007a583          	lw	a1,0(a5)
 3a4:	000a8513          	mv	a0,s5
 3a8:	00000097          	auipc	ra,0x0
 3ac:	db8080e7          	jalr	-584(ra) # 160 <printint>
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
 3b0:	00000993          	li	s3,0
 3b4:	f6dff06f          	j	320 <vprintf+0x58>
        printint(fd, va_arg(ap, uint64), 10, 0);
 3b8:	fb843783          	ld	a5,-72(s0)
 3bc:	00878713          	addi	a4,a5,8
 3c0:	fae43c23          	sd	a4,-72(s0)
 3c4:	00000693          	li	a3,0
 3c8:	00a00613          	li	a2,10
 3cc:	0007a583          	lw	a1,0(a5)
 3d0:	000a8513          	mv	a0,s5
 3d4:	00000097          	auipc	ra,0x0
 3d8:	d8c080e7          	jalr	-628(ra) # 160 <printint>
      state = 0;
 3dc:	00000993          	li	s3,0
 3e0:	f41ff06f          	j	320 <vprintf+0x58>
        printint(fd, va_arg(ap, int), 16, 0);
 3e4:	fb843783          	ld	a5,-72(s0)
 3e8:	00878713          	addi	a4,a5,8
 3ec:	fae43c23          	sd	a4,-72(s0)
 3f0:	00000693          	li	a3,0
 3f4:	01000613          	li	a2,16
 3f8:	0007a583          	lw	a1,0(a5)
 3fc:	000a8513          	mv	a0,s5
 400:	00000097          	auipc	ra,0x0
 404:	d60080e7          	jalr	-672(ra) # 160 <printint>
      state = 0;
 408:	00000993          	li	s3,0
 40c:	f15ff06f          	j	320 <vprintf+0x58>
        printptr(fd, va_arg(ap, uint64));
 410:	fb843783          	ld	a5,-72(s0)
 414:	00878713          	addi	a4,a5,8
 418:	fae43c23          	sd	a4,-72(s0)
 41c:	0007b583          	ld	a1,0(a5)
 420:	000a8513          	mv	a0,s5
 424:	00000097          	auipc	ra,0x0
 428:	e0c080e7          	jalr	-500(ra) # 230 <printptr>
      state = 0;
 42c:	00000993          	li	s3,0
 430:	ef1ff06f          	j	320 <vprintf+0x58>
        s = va_arg(ap, char*);
 434:	fb843783          	ld	a5,-72(s0)
 438:	00878713          	addi	a4,a5,8
 43c:	fae43c23          	sd	a4,-72(s0)
 440:	0007b483          	ld	s1,0(a5)
        if(s == 0)
 444:	02049063          	bnez	s1,464 <vprintf+0x19c>
          s = "(null)";
 448:	00001497          	auipc	s1,0x1
 44c:	8c048493          	addi	s1,s1,-1856 # d08 <digits+0x18>
 450:	0140006f          	j	464 <vprintf+0x19c>
          putc(fd, *s);
 454:	000a8513          	mv	a0,s5
 458:	00000097          	auipc	ra,0x0
 45c:	cd4080e7          	jalr	-812(ra) # 12c <putc>
          s++;
 460:	00148493          	addi	s1,s1,1
        while(*s != 0){
 464:	0004c583          	lbu	a1,0(s1)
 468:	fe0596e3          	bnez	a1,454 <vprintf+0x18c>
      state = 0;
 46c:	00000993          	li	s3,0
 470:	eb1ff06f          	j	320 <vprintf+0x58>
        putc(fd, va_arg(ap, uint));
 474:	fb843783          	ld	a5,-72(s0)
 478:	00878713          	addi	a4,a5,8
 47c:	fae43c23          	sd	a4,-72(s0)
 480:	0007c583          	lbu	a1,0(a5)
 484:	000a8513          	mv	a0,s5
 488:	00000097          	auipc	ra,0x0
 48c:	ca4080e7          	jalr	-860(ra) # 12c <putc>
      state = 0;
 490:	00000993          	li	s3,0
 494:	e8dff06f          	j	320 <vprintf+0x58>
        putc(fd, c);
 498:	00048593          	mv	a1,s1
 49c:	000a8513          	mv	a0,s5
 4a0:	00000097          	auipc	ra,0x0
 4a4:	c8c080e7          	jalr	-884(ra) # 12c <putc>
      state = 0;
 4a8:	00000993          	li	s3,0
 4ac:	e75ff06f          	j	320 <vprintf+0x58>
        putc(fd, '%');
 4b0:	02500593          	li	a1,37
 4b4:	000a8513          	mv	a0,s5
 4b8:	00000097          	auipc	ra,0x0
 4bc:	c74080e7          	jalr	-908(ra) # 12c <putc>
        putc(fd, c);
 4c0:	00048593          	mv	a1,s1
 4c4:	000a8513          	mv	a0,s5
 4c8:	00000097          	auipc	ra,0x0
 4cc:	c64080e7          	jalr	-924(ra) # 12c <putc>
      state = 0;
 4d0:	00000993          	li	s3,0
 4d4:	e4dff06f          	j	320 <vprintf+0x58>
    }
  }
}
 4d8:	04813083          	ld	ra,72(sp)
 4dc:	04013403          	ld	s0,64(sp)
 4e0:	03813483          	ld	s1,56(sp)
 4e4:	03013903          	ld	s2,48(sp)
 4e8:	02813983          	ld	s3,40(sp)
 4ec:	02013a03          	ld	s4,32(sp)
 4f0:	01813a83          	ld	s5,24(sp)
 4f4:	05010113          	addi	sp,sp,80
 4f8:	00008067          	ret

00000000000004fc <fprintf>:

void
fprintf(int fd, const char *fmt, ...)
{
 4fc:	fb010113          	addi	sp,sp,-80
 500:	00113c23          	sd	ra,24(sp)
 504:	00813823          	sd	s0,16(sp)
 508:	02010413          	addi	s0,sp,32
 50c:	00c43023          	sd	a2,0(s0)
 510:	00d43423          	sd	a3,8(s0)
 514:	00e43823          	sd	a4,16(s0)
 518:	00f43c23          	sd	a5,24(s0)
 51c:	03043023          	sd	a6,32(s0)
 520:	03143423          	sd	a7,40(s0)
  va_list ap;

  va_start(ap, fmt);
 524:	fe843423          	sd	s0,-24(s0)
  vprintf(fd, fmt, ap);
 528:	00040613          	mv	a2,s0
 52c:	00000097          	auipc	ra,0x0
 530:	d9c080e7          	jalr	-612(ra) # 2c8 <vprintf>
}
 534:	01813083          	ld	ra,24(sp)
 538:	01013403          	ld	s0,16(sp)
 53c:	05010113          	addi	sp,sp,80
 540:	00008067          	ret

0000000000000544 <printf>:

void
printf(const char *fmt, ...)
{
 544:	fa010113          	addi	sp,sp,-96
 548:	00113c23          	sd	ra,24(sp)
 54c:	00813823          	sd	s0,16(sp)
 550:	02010413          	addi	s0,sp,32
 554:	00b43423          	sd	a1,8(s0)
 558:	00c43823          	sd	a2,16(s0)
 55c:	00d43c23          	sd	a3,24(s0)
 560:	02e43023          	sd	a4,32(s0)
 564:	02f43423          	sd	a5,40(s0)
 568:	03043823          	sd	a6,48(s0)
 56c:	03143c23          	sd	a7,56(s0)
  va_list ap;

  va_start(ap, fmt);
 570:	00840613          	addi	a2,s0,8
 574:	fec43423          	sd	a2,-24(s0)
  vprintf(1, fmt, ap);
 578:	00050593          	mv	a1,a0
 57c:	00100513          	li	a0,1
 580:	00000097          	auipc	ra,0x0
 584:	d48080e7          	jalr	-696(ra) # 2c8 <vprintf>
}
 588:	01813083          	ld	ra,24(sp)
 58c:	01013403          	ld	s0,16(sp)
 590:	06010113          	addi	sp,sp,96
 594:	00008067          	ret

0000000000000598 <strcpy>:
#include "fs/fcntl.h"
#include "user.h"

char*
strcpy(char *s, const char *t)
{
 598:	ff010113          	addi	sp,sp,-16
 59c:	00813423          	sd	s0,8(sp)
 5a0:	01010413          	addi	s0,sp,16
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
 5a4:	00050693          	mv	a3,a0
 5a8:	0005c703          	lbu	a4,0(a1)
 5ac:	00158593          	addi	a1,a1,1
 5b0:	00e68023          	sb	a4,0(a3)
 5b4:	00168693          	addi	a3,a3,1
 5b8:	fe0718e3          	bnez	a4,5a8 <strcpy+0x10>
    ;
  return os;
}
 5bc:	00813403          	ld	s0,8(sp)
 5c0:	01010113          	addi	sp,sp,16
 5c4:	00008067          	ret

00000000000005c8 <strcmp>:

int
strcmp(const char *p, const char *q)
{
 5c8:	ff010113          	addi	sp,sp,-16
 5cc:	00813423          	sd	s0,8(sp)
 5d0:	01010413          	addi	s0,sp,16
  while(*p && *p == *q)
 5d4:	00c0006f          	j	5e0 <strcmp+0x18>
    p++, q++;
 5d8:	00150513          	addi	a0,a0,1
 5dc:	00158593          	addi	a1,a1,1
  while(*p && *p == *q)
 5e0:	00054783          	lbu	a5,0(a0)
 5e4:	00078663          	beqz	a5,5f0 <strcmp+0x28>
 5e8:	0005c703          	lbu	a4,0(a1)
 5ec:	fee786e3          	beq	a5,a4,5d8 <strcmp+0x10>
  return (uchar)*p - (uchar)*q;
 5f0:	0005c503          	lbu	a0,0(a1)
}
 5f4:	40a7853b          	subw	a0,a5,a0
 5f8:	00813403          	ld	s0,8(sp)
 5fc:	01010113          	addi	sp,sp,16
 600:	00008067          	ret

0000000000000604 <strlen>:

uint
strlen(const char *s)
{
 604:	ff010113          	addi	sp,sp,-16
 608:	00813423          	sd	s0,8(sp)
 60c:	01010413          	addi	s0,sp,16
 610:	00050713          	mv	a4,a0
  int n;

  for(n = 0; s[n]; n++)
 614:	00000513          	li	a0,0
 618:	0080006f          	j	620 <strlen+0x1c>
 61c:	0015051b          	addiw	a0,a0,1
 620:	00a707b3          	add	a5,a4,a0
 624:	0007c783          	lbu	a5,0(a5)
 628:	fe079ae3          	bnez	a5,61c <strlen+0x18>
    ;
  return n;
}
 62c:	00813403          	ld	s0,8(sp)
 630:	01010113          	addi	sp,sp,16
 634:	00008067          	ret

0000000000000638 <memset>:

void*
memset(void *dst, int c, uint n)
{
 638:	ff010113          	addi	sp,sp,-16
 63c:	00813423          	sd	s0,8(sp)
 640:	01010413          	addi	s0,sp,16
  char *cdst = (char *) dst;
  int i;
  for(i = 0; i < n; i++){
 644:	00000793          	li	a5,0
 648:	0100006f          	j	658 <memset+0x20>
    cdst[i] = c;
 64c:	00f50733          	add	a4,a0,a5
 650:	00b70023          	sb	a1,0(a4)
  for(i = 0; i < n; i++){
 654:	0017879b          	addiw	a5,a5,1
 658:	0007871b          	sext.w	a4,a5
 65c:	fec768e3          	bltu	a4,a2,64c <memset+0x14>
  }
  return dst;
}
 660:	00813403          	ld	s0,8(sp)
 664:	01010113          	addi	sp,sp,16
 668:	00008067          	ret

000000000000066c <strchr>:

char*
strchr(const char *s, char c)
{
 66c:	ff010113          	addi	sp,sp,-16
 670:	00813423          	sd	s0,8(sp)
 674:	01010413          	addi	s0,sp,16
  for(; *s; s++)
 678:	0080006f          	j	680 <strchr+0x14>
 67c:	00150513          	addi	a0,a0,1
 680:	00054783          	lbu	a5,0(a0)
 684:	00078663          	beqz	a5,690 <strchr+0x24>
    if(*s == c)
 688:	feb79ae3          	bne	a5,a1,67c <strchr+0x10>
 68c:	0080006f          	j	694 <strchr+0x28>
      return (char*)s;
  return 0;
 690:	00000513          	li	a0,0
}
 694:	00813403          	ld	s0,8(sp)
 698:	01010113          	addi	sp,sp,16
 69c:	00008067          	ret

00000000000006a0 <gets>:

char*
gets(char *buf, int max)
{
 6a0:	fc010113          	addi	sp,sp,-64
 6a4:	02113c23          	sd	ra,56(sp)
 6a8:	02813823          	sd	s0,48(sp)
 6ac:	02913423          	sd	s1,40(sp)
 6b0:	03213023          	sd	s2,32(sp)
 6b4:	01313c23          	sd	s3,24(sp)
 6b8:	01413823          	sd	s4,16(sp)
 6bc:	04010413          	addi	s0,sp,64
 6c0:	00050993          	mv	s3,a0
 6c4:	00058a13          	mv	s4,a1
  int i, cc;
  char c;

  for(i=0; i+1 < max; ){
 6c8:	00000913          	li	s2,0
 6cc:	00090493          	mv	s1,s2
 6d0:	0019091b          	addiw	s2,s2,1
 6d4:	05495263          	bge	s2,s4,718 <gets+0x78>
    cc = read(0, &c, 1);
 6d8:	00100613          	li	a2,1
 6dc:	fcf40593          	addi	a1,s0,-49
 6e0:	00000513          	li	a0,0
 6e4:	00000097          	auipc	ra,0x0
 6e8:	434080e7          	jalr	1076(ra) # b18 <read>
    if(cc < 1)
 6ec:	02a05663          	blez	a0,718 <gets+0x78>
      break;
    buf[i++] = c;
 6f0:	009984b3          	add	s1,s3,s1
 6f4:	fcf44783          	lbu	a5,-49(s0)
 6f8:	00f48023          	sb	a5,0(s1)
    if(c == '\n' || c == '\r')
 6fc:	00a00713          	li	a4,10
 700:	00e78a63          	beq	a5,a4,714 <gets+0x74>
 704:	00d00713          	li	a4,13
 708:	fce792e3          	bne	a5,a4,6cc <gets+0x2c>
    buf[i++] = c;
 70c:	00090493          	mv	s1,s2
 710:	0080006f          	j	718 <gets+0x78>
 714:	00090493          	mv	s1,s2
      break;
  }
  buf[i] = '\0';
 718:	009984b3          	add	s1,s3,s1
 71c:	00048023          	sb	zero,0(s1)
  return buf;
}
 720:	00098513          	mv	a0,s3
 724:	03813083          	ld	ra,56(sp)
 728:	03013403          	ld	s0,48(sp)
 72c:	02813483          	ld	s1,40(sp)
 730:	02013903          	ld	s2,32(sp)
 734:	01813983          	ld	s3,24(sp)
 738:	01013a03          	ld	s4,16(sp)
 73c:	04010113          	addi	sp,sp,64
 740:	00008067          	ret

0000000000000744 <stat>:

int
stat(const char *n, struct stat *st)
{
 744:	fe010113          	addi	sp,sp,-32
 748:	00113c23          	sd	ra,24(sp)
 74c:	00813823          	sd	s0,16(sp)
 750:	00913423          	sd	s1,8(sp)
 754:	01213023          	sd	s2,0(sp)
 758:	02010413          	addi	s0,sp,32
 75c:	00058913          	mv	s2,a1
  int fd;
  int r;

  fd = open(n, O_RDONLY);
 760:	00000593          	li	a1,0
 764:	00000097          	auipc	ra,0x0
 768:	42c080e7          	jalr	1068(ra) # b90 <open>
  if(fd < 0)
 76c:	04054063          	bltz	a0,7ac <stat+0x68>
 770:	00050493          	mv	s1,a0
    return -1;
  r = fstat(fd, st);
 774:	00090593          	mv	a1,s2
 778:	00000097          	auipc	ra,0x0
 77c:	3c4080e7          	jalr	964(ra) # b3c <fstat>
 780:	00050913          	mv	s2,a0
  close(fd);
 784:	00048513          	mv	a0,s1
 788:	00000097          	auipc	ra,0x0
 78c:	450080e7          	jalr	1104(ra) # bd8 <close>
  return r;
}
 790:	00090513          	mv	a0,s2
 794:	01813083          	ld	ra,24(sp)
 798:	01013403          	ld	s0,16(sp)
 79c:	00813483          	ld	s1,8(sp)
 7a0:	00013903          	ld	s2,0(sp)
 7a4:	02010113          	addi	sp,sp,32
 7a8:	00008067          	ret
    return -1;
 7ac:	fff00913          	li	s2,-1
 7b0:	fe1ff06f          	j	790 <stat+0x4c>

00000000000007b4 <atoi>:

int
atoi(const char *s)
{
 7b4:	ff010113          	addi	sp,sp,-16
 7b8:	00813423          	sd	s0,8(sp)
 7bc:	01010413          	addi	s0,sp,16
 7c0:	00050713          	mv	a4,a0
  int n;

  n = 0;
 7c4:	00000513          	li	a0,0
  while('0' <= *s && *s <= '9')
 7c8:	01c0006f          	j	7e4 <atoi+0x30>
    n = n*10 + *s++ - '0';
 7cc:	0025179b          	slliw	a5,a0,0x2
 7d0:	00a787bb          	addw	a5,a5,a0
 7d4:	0017979b          	slliw	a5,a5,0x1
 7d8:	00170713          	addi	a4,a4,1
 7dc:	00d787bb          	addw	a5,a5,a3
 7e0:	fd07851b          	addiw	a0,a5,-48
  while('0' <= *s && *s <= '9')
 7e4:	00074683          	lbu	a3,0(a4)
 7e8:	fd06879b          	addiw	a5,a3,-48
 7ec:	0ff7f793          	zext.b	a5,a5
 7f0:	00900613          	li	a2,9
 7f4:	fcf67ce3          	bgeu	a2,a5,7cc <atoi+0x18>
  return n;
}
 7f8:	00813403          	ld	s0,8(sp)
 7fc:	01010113          	addi	sp,sp,16
 800:	00008067          	ret

0000000000000804 <memmove>:

void*
memmove(void *vdst, const void *vsrc, int n)
{
 804:	ff010113          	addi	sp,sp,-16
 808:	00813423          	sd	s0,8(sp)
 80c:	01010413          	addi	s0,sp,16
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  if (src > dst) {
 810:	02b56c63          	bltu	a0,a1,848 <memmove+0x44>
    while(n-- > 0)
      *dst++ = *src++;
  } else {
    dst += n;
 814:	00c507b3          	add	a5,a0,a2
    src += n;
 818:	00c585b3          	add	a1,a1,a2
    while(n-- > 0)
 81c:	0480006f          	j	864 <memmove+0x60>
      *dst++ = *src++;
 820:	0005c683          	lbu	a3,0(a1)
 824:	00d78023          	sb	a3,0(a5)
 828:	00158593          	addi	a1,a1,1
 82c:	00178793          	addi	a5,a5,1
    while(n-- > 0)
 830:	00070613          	mv	a2,a4
 834:	fff6071b          	addiw	a4,a2,-1
 838:	fec044e3          	bgtz	a2,820 <memmove+0x1c>
      *--dst = *--src;
  }
  return vdst;
}
 83c:	00813403          	ld	s0,8(sp)
 840:	01010113          	addi	sp,sp,16
 844:	00008067          	ret
  dst = vdst;
 848:	00050793          	mv	a5,a0
 84c:	fe9ff06f          	j	834 <memmove+0x30>
      *--dst = *--src;
 850:	fff58593          	addi	a1,a1,-1
 854:	fff78793          	addi	a5,a5,-1
 858:	0005c683          	lbu	a3,0(a1)
 85c:	00d78023          	sb	a3,0(a5)
    while(n-- > 0)
 860:	00070613          	mv	a2,a4
 864:	fff6071b          	addiw	a4,a2,-1
 868:	fec044e3          	bgtz	a2,850 <memmove+0x4c>
 86c:	fd1ff06f          	j	83c <memmove+0x38>

0000000000000870 <memcmp>:

int
memcmp(const void *s1, const void *s2, uint n)
{
 870:	ff010113          	addi	sp,sp,-16
 874:	00813423          	sd	s0,8(sp)
 878:	01010413          	addi	s0,sp,16
  const char *p1 = s1, *p2 = s2;
  while (n-- > 0) {
 87c:	0100006f          	j	88c <memcmp+0x1c>
    if (*p1 != *p2) {
      return *p1 - *p2;
    }
    p1++;
 880:	00150513          	addi	a0,a0,1
    p2++;
 884:	00158593          	addi	a1,a1,1
  while (n-- > 0) {
 888:	00068613          	mv	a2,a3
 88c:	fff6069b          	addiw	a3,a2,-1
 890:	00060c63          	beqz	a2,8a8 <memcmp+0x38>
    if (*p1 != *p2) {
 894:	00054783          	lbu	a5,0(a0)
 898:	0005c703          	lbu	a4,0(a1)
 89c:	fee782e3          	beq	a5,a4,880 <memcmp+0x10>
      return *p1 - *p2;
 8a0:	40e7853b          	subw	a0,a5,a4
 8a4:	0080006f          	j	8ac <memcmp+0x3c>
  }
  return 0;
 8a8:	00000513          	li	a0,0
}
 8ac:	00813403          	ld	s0,8(sp)
 8b0:	01010113          	addi	sp,sp,16
 8b4:	00008067          	ret

00000000000008b8 <memcpy>:

void *
memcpy(void *dst, const void *src, uint n)
{
 8b8:	ff010113          	addi	sp,sp,-16
 8bc:	00113423          	sd	ra,8(sp)
 8c0:	00813023          	sd	s0,0(sp)
 8c4:	01010413          	addi	s0,sp,16
  return memmove(dst, src, n);
 8c8:	00000097          	auipc	ra,0x0
 8cc:	f3c080e7          	jalr	-196(ra) # 804 <memmove>
}
 8d0:	00813083          	ld	ra,8(sp)
 8d4:	00013403          	ld	s0,0(sp)
 8d8:	01010113          	addi	sp,sp,16
 8dc:	00008067          	ret

00000000000008e0 <free>:
static Header base;
static Header *freep;

void
free(void *ap)
{
 8e0:	ff010113          	addi	sp,sp,-16
 8e4:	00813423          	sd	s0,8(sp)
 8e8:	01010413          	addi	s0,sp,16
  Header *bp, *p;

  bp = (Header*)ap - 1;
 8ec:	ff050693          	addi	a3,a0,-16
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
 8f0:	00000797          	auipc	a5,0x0
 8f4:	4307b783          	ld	a5,1072(a5) # d20 <freep>
 8f8:	0080006f          	j	900 <free+0x20>
 8fc:	00070793          	mv	a5,a4
 900:	00d7f663          	bgeu	a5,a3,90c <free+0x2c>
 904:	0007b703          	ld	a4,0(a5)
 908:	00e6ee63          	bltu	a3,a4,924 <free+0x44>
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
 90c:	0007b703          	ld	a4,0(a5)
 910:	fee7e6e3          	bltu	a5,a4,8fc <free+0x1c>
 914:	00d7e863          	bltu	a5,a3,924 <free+0x44>
 918:	00e6e663          	bltu	a3,a4,924 <free+0x44>
 91c:	00070793          	mv	a5,a4
 920:	fe1ff06f          	j	900 <free+0x20>
      break;
  if(bp + bp->s.size == p->s.ptr){
 924:	ff852583          	lw	a1,-8(a0)
 928:	02059613          	slli	a2,a1,0x20
 92c:	01c65713          	srli	a4,a2,0x1c
 930:	00e68733          	add	a4,a3,a4
 934:	0007b603          	ld	a2,0(a5)
 938:	02c70a63          	beq	a4,a2,96c <free+0x8c>
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
 93c:	fec53823          	sd	a2,-16(a0)
  if(p + p->s.size == bp){
 940:	0087a603          	lw	a2,8(a5)
 944:	02061593          	slli	a1,a2,0x20
 948:	01c5d713          	srli	a4,a1,0x1c
 94c:	00e78733          	add	a4,a5,a4
 950:	02d70c63          	beq	a4,a3,988 <free+0xa8>
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
 954:	00d7b023          	sd	a3,0(a5)
  freep = p;
 958:	00000717          	auipc	a4,0x0
 95c:	3cf73423          	sd	a5,968(a4) # d20 <freep>
}
 960:	00813403          	ld	s0,8(sp)
 964:	01010113          	addi	sp,sp,16
 968:	00008067          	ret
    bp->s.size += p->s.ptr->s.size;
 96c:	00862703          	lw	a4,8(a2)
 970:	00e585bb          	addw	a1,a1,a4
 974:	feb52c23          	sw	a1,-8(a0)
    bp->s.ptr = p->s.ptr->s.ptr;
 978:	0007b703          	ld	a4,0(a5)
 97c:	00073703          	ld	a4,0(a4)
 980:	fee53823          	sd	a4,-16(a0)
 984:	fbdff06f          	j	940 <free+0x60>
    p->s.size += bp->s.size;
 988:	ff852703          	lw	a4,-8(a0)
 98c:	00e6063b          	addw	a2,a2,a4
 990:	00c7a423          	sw	a2,8(a5)
    p->s.ptr = bp->s.ptr;
 994:	ff053703          	ld	a4,-16(a0)
 998:	00e7b023          	sd	a4,0(a5)
 99c:	fbdff06f          	j	958 <free+0x78>

00000000000009a0 <morecore>:

static Header*
morecore(uint nu)
{
 9a0:	fe010113          	addi	sp,sp,-32
 9a4:	00113c23          	sd	ra,24(sp)
 9a8:	00813823          	sd	s0,16(sp)
 9ac:	00913423          	sd	s1,8(sp)
 9b0:	02010413          	addi	s0,sp,32
 9b4:	00050493          	mv	s1,a0
  char *p;
  Header *hp;

  if(nu < 4096)
 9b8:	000017b7          	lui	a5,0x1
 9bc:	00f57463          	bgeu	a0,a5,9c4 <morecore+0x24>
    nu = 4096;
 9c0:	000014b7          	lui	s1,0x1
  p = sbrk(nu * sizeof(Header));
 9c4:	0044951b          	slliw	a0,s1,0x4
 9c8:	00000097          	auipc	ra,0x0
 9cc:	1a4080e7          	jalr	420(ra) # b6c <sbrk>
  if(p == (char*)-1)
 9d0:	fff00793          	li	a5,-1
 9d4:	02f50863          	beq	a0,a5,a04 <morecore+0x64>
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
 9d8:	00952423          	sw	s1,8(a0)
  free((void*)(hp + 1));
 9dc:	01050513          	addi	a0,a0,16
 9e0:	00000097          	auipc	ra,0x0
 9e4:	f00080e7          	jalr	-256(ra) # 8e0 <free>
  return freep;
 9e8:	00000517          	auipc	a0,0x0
 9ec:	33853503          	ld	a0,824(a0) # d20 <freep>
}
 9f0:	01813083          	ld	ra,24(sp)
 9f4:	01013403          	ld	s0,16(sp)
 9f8:	00813483          	ld	s1,8(sp)
 9fc:	02010113          	addi	sp,sp,32
 a00:	00008067          	ret
    return 0;
 a04:	00000513          	li	a0,0
 a08:	fe9ff06f          	j	9f0 <morecore+0x50>

0000000000000a0c <malloc>:

void*
malloc(uint nbytes)
{
 a0c:	fe010113          	addi	sp,sp,-32
 a10:	00113c23          	sd	ra,24(sp)
 a14:	00813823          	sd	s0,16(sp)
 a18:	00913423          	sd	s1,8(sp)
 a1c:	01213023          	sd	s2,0(sp)
 a20:	02010413          	addi	s0,sp,32
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
 a24:	02051493          	slli	s1,a0,0x20
 a28:	0204d493          	srli	s1,s1,0x20
 a2c:	00f48493          	addi	s1,s1,15 # 100f <__BSS_END__+0x2d7>
 a30:	0044d493          	srli	s1,s1,0x4
 a34:	0014891b          	addiw	s2,s1,1
 a38:	00148493          	addi	s1,s1,1
  if((prevp = freep) == 0){
 a3c:	00000717          	auipc	a4,0x0
 a40:	2e473703          	ld	a4,740(a4) # d20 <freep>
 a44:	00070663          	beqz	a4,a50 <malloc+0x44>
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 a48:	00073503          	ld	a0,0(a4)
 a4c:	0740006f          	j	ac0 <malloc+0xb4>
    base.s.ptr = freep = prevp = &base;
 a50:	00000717          	auipc	a4,0x0
 a54:	2d870713          	addi	a4,a4,728 # d28 <base>
 a58:	00000797          	auipc	a5,0x0
 a5c:	2ce7b423          	sd	a4,712(a5) # d20 <freep>
 a60:	00e73023          	sd	a4,0(a4)
    base.s.size = 0;
 a64:	00072423          	sw	zero,8(a4)
 a68:	fe1ff06f          	j	a48 <malloc+0x3c>
    if(p->s.size >= nunits){
      if(p->s.size == nunits)
 a6c:	04978063          	beq	a5,s1,aac <malloc+0xa0>
        prevp->s.ptr = p->s.ptr;
      else {
        p->s.size -= nunits;
 a70:	412787bb          	subw	a5,a5,s2
 a74:	00f52423          	sw	a5,8(a0)
        p += p->s.size;
 a78:	02079693          	slli	a3,a5,0x20
 a7c:	01c6d793          	srli	a5,a3,0x1c
 a80:	00f50533          	add	a0,a0,a5
        p->s.size = nunits;
 a84:	01252423          	sw	s2,8(a0)
      }
      freep = prevp;
 a88:	00000797          	auipc	a5,0x0
 a8c:	28e7bc23          	sd	a4,664(a5) # d20 <freep>
      return (void*)(p + 1);
 a90:	01050513          	addi	a0,a0,16
    }
    if(p == freep)
      if((p = morecore(nunits)) == 0)
        return 0;
  }
}
 a94:	01813083          	ld	ra,24(sp)
 a98:	01013403          	ld	s0,16(sp)
 a9c:	00813483          	ld	s1,8(sp)
 aa0:	00013903          	ld	s2,0(sp)
 aa4:	02010113          	addi	sp,sp,32
 aa8:	00008067          	ret
        prevp->s.ptr = p->s.ptr;
 aac:	00053783          	ld	a5,0(a0)
 ab0:	00f73023          	sd	a5,0(a4)
 ab4:	fd5ff06f          	j	a88 <malloc+0x7c>
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
 ab8:	00050713          	mv	a4,a0
 abc:	00053503          	ld	a0,0(a0)
    if(p->s.size >= nunits){
 ac0:	00852783          	lw	a5,8(a0)
 ac4:	fa97f4e3          	bgeu	a5,s1,a6c <malloc+0x60>
    if(p == freep)
 ac8:	00000797          	auipc	a5,0x0
 acc:	2587b783          	ld	a5,600(a5) # d20 <freep>
 ad0:	fea794e3          	bne	a5,a0,ab8 <malloc+0xac>
      if((p = morecore(nunits)) == 0)
 ad4:	00048513          	mv	a0,s1
 ad8:	00000097          	auipc	ra,0x0
 adc:	ec8080e7          	jalr	-312(ra) # 9a0 <morecore>
 ae0:	fc051ce3          	bnez	a0,ab8 <malloc+0xac>
 ae4:	fb1ff06f          	j	a94 <malloc+0x88>

0000000000000ae8 <fork>:

#ifndef __SYS_CALL
#define __SYS_CALL
#endif

__SYS_CALL(1, fork, sys_fork)
 ae8:	00100893          	li	a7,1
 aec:	00000073          	ecall
 af0:	00008067          	ret

0000000000000af4 <exit>:
__SYS_CALL(2, exit, sys_exit)
 af4:	00200893          	li	a7,2
 af8:	00000073          	ecall
 afc:	00008067          	ret

0000000000000b00 <wait>:
__SYS_CALL(3, wait, sys_wait)
 b00:	00300893          	li	a7,3
 b04:	00000073          	ecall
 b08:	00008067          	ret

0000000000000b0c <pipe>:
__SYS_CALL(4, pipe, sys_pipe)
 b0c:	00400893          	li	a7,4
 b10:	00000073          	ecall
 b14:	00008067          	ret

0000000000000b18 <read>:
__SYS_CALL(5, read, sys_read)
 b18:	00500893          	li	a7,5
 b1c:	00000073          	ecall
 b20:	00008067          	ret

0000000000000b24 <kill>:
__SYS_CALL(6, kill, sys_kill)
 b24:	00600893          	li	a7,6
 b28:	00000073          	ecall
 b2c:	00008067          	ret

0000000000000b30 <exec>:
__SYS_CALL(7, exec, sys_exec)
 b30:	00700893          	li	a7,7
 b34:	00000073          	ecall
 b38:	00008067          	ret

0000000000000b3c <fstat>:
__SYS_CALL(8, fstat, sys_fstat)
 b3c:	00800893          	li	a7,8
 b40:	00000073          	ecall
 b44:	00008067          	ret

0000000000000b48 <chdir>:
__SYS_CALL(9, chdir, sys_chdir)
 b48:	00900893          	li	a7,9
 b4c:	00000073          	ecall
 b50:	00008067          	ret

0000000000000b54 <dup>:
__SYS_CALL(10, dup, sys_dup)
 b54:	00a00893          	li	a7,10
 b58:	00000073          	ecall
 b5c:	00008067          	ret

0000000000000b60 <getpid>:
__SYS_CALL(11, getpid, sys_getpid)
 b60:	00b00893          	li	a7,11
 b64:	00000073          	ecall
 b68:	00008067          	ret

0000000000000b6c <sbrk>:
__SYS_CALL(12, sbrk, sys_sbrk)
 b6c:	00c00893          	li	a7,12
 b70:	00000073          	ecall
 b74:	00008067          	ret

0000000000000b78 <sleep>:
__SYS_CALL(13, sleep, sys_sleep)
 b78:	00d00893          	li	a7,13
 b7c:	00000073          	ecall
 b80:	00008067          	ret

0000000000000b84 <uptime>:
__SYS_CALL(14, uptime, sys_uptime)
 b84:	00e00893          	li	a7,14
 b88:	00000073          	ecall
 b8c:	00008067          	ret

0000000000000b90 <open>:
__SYS_CALL(15, open, sys_open)
 b90:	00f00893          	li	a7,15
 b94:	00000073          	ecall
 b98:	00008067          	ret

0000000000000b9c <write>:
__SYS_CALL(16, write, sys_write)
 b9c:	01000893          	li	a7,16
 ba0:	00000073          	ecall
 ba4:	00008067          	ret

0000000000000ba8 <mknod>:
__SYS_CALL(17, mknod, sys_mknod)
 ba8:	01100893          	li	a7,17
 bac:	00000073          	ecall
 bb0:	00008067          	ret

0000000000000bb4 <unlink>:
__SYS_CALL(18, unlink, sys_unlink)
 bb4:	01200893          	li	a7,18
 bb8:	00000073          	ecall
 bbc:	00008067          	ret

0000000000000bc0 <link>:
__SYS_CALL(19, link, sys_link)
 bc0:	01300893          	li	a7,19
 bc4:	00000073          	ecall
 bc8:	00008067          	ret

0000000000000bcc <mkdir>:
__SYS_CALL(20, mkdir, sys_mkdir)
 bcc:	01400893          	li	a7,20
 bd0:	00000073          	ecall
 bd4:	00008067          	ret

0000000000000bd8 <close>:
__SYS_CALL(21, close, sys_close)
 bd8:	01500893          	li	a7,21
 bdc:	00000073          	ecall
 be0:	00008067          	ret

0000000000000be4 <ktest>:
__SYS_CALL(88, ktest, sys_ktest)
 be4:	05800893          	li	a7,88
 be8:	00000073          	ecall
 bec:	00008067          	ret

0000000000000bf0 <timetag>:
 bf0:	05900893          	li	a7,89
 bf4:	00000073          	ecall
 bf8:	00008067          	ret
