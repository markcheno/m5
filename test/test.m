MODULE test;

TYPE
	Point3d = RECORD
		x,y,z: INTEGER;
	END;

VAR
	bool: BOOLEAN;

	r: REAL;
	
	a,b,x,y,z: INTEGER;

    p1,p2: Point3d;


PROCEDURE proc1(): REAL;
BEGIN
END proc1;

PROCEDURE func1(x,y,z: INTEGER): INTEGER;
BEGIN
END func1;


BEGIN

IF x > 10 THEN
 y:=0;
END;


x := -3 - 2;

bool := ~ (y >10);

r := ((-74-10)/32)*(23+17);
x := ((-74-10)MOD 32)*(23+17);

y := 69;

a := b;

p1.x := 0;
p1.y := 0;
p1.z := 0;

REPEAT
 x := x+1;
 y := y+1;
 z := z+1
UNTIL y=10;

WHILE x <= 10 DO
 x := x+1;
 y := y+1;
 z := z+1
END;

LOOP
 x := x+1;
 y := y+1;
 z := z+1;
 IF y >= 1000 THEN EXIT END
END;

IF x < 1000 THEN 
  y := 1000; z:=1
ELSIF x < 100 THEN
  y := 100; z:=1
ELSIF x < 10 THEN
  y := 10; z:=1
ELSIF x < 109 THEN
  y := 10
ELSE
  z:=1;
  IF y # 0 THEN
    y := 0
  ELSE
    x := 1
  END
END;

IF y # 1 THEN
  x := 0;
  z := 0;
ELSIF y > 10 THEN
  x := 1;
  z := 1;
ELSIF y > 100 THEN
  x := 2;
  z := 2;
ELSIF y > 1000 THEN
  x := 3;
  z := 3;
ELSE
  x := 4;
  z := 4;
END;

FOR x:=10 TO 0 BY -3 DO
 bool := ~ (y=0);
 z := z+1;  
END;


CASE x OF
| 1,2   : y:=1;  z:=2
| 3     : y:=3;  z:=4
| 5,6   : y:=5;  z:=6
| -7,-8 : y:=-7; z:=-8
ELSE      y:=0;  z:=0
END;

proc1;
r := proc1;
x := 1 + (2 + 3);
r := proc1 + func1(3-1,2*2,1+4);
func1(3,2,1);
x := func1(3-1,2*2,1+4);
x := 1 + (func1(1,2,3) + 2);


END test.
