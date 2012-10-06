module xxx;

const
 a = 1=1; (* BOOLEAN,TRUE *)
 b = 1=0; (* BOOLEAN,FALSE *)
 
 d = 1#0; (* BOOLEAN,TRUE *)
 c = 1#1; (* BOOLEAN,FALSE *)
 
 e = 1<2; (* BOOLEAN,TRUE *)
 f = 2<1; (* BOOLEAN,FALSE *)

 g = 1<=1; (* BOOLEAN,TRUE *)
 h = 2<=1; (* BOOLEAN,FALSE *)

 i = 2>1; (* BOOLEAN,TRUE *)
 j = 1>2; (* BOOLEAN,FALSE *)

 k = 2>=2; (* BOOLEAN,TRUE *)
 l = 2>=3; (* BOOLEAN,FALSE *)
 
 n = (1=1) OR (1=0); (* BOOLEAN,TRUE *)
 m = (1=2) OR (1=3); (* BOOLEAN,FALSE *)
 
 o = (1=1) & (2=2); (* BOOLEAN,TRUE *)
 p = (1=1) & (1=2); (* BOOLEAN,FALSE *)

 q = 69-9;  (* INTEGER, 60 *)
 r = 10-20; (* INTEGER, -10 *)

 s = 50-20.0; (* REAL,30.0 *)
 t = 30.0-50; (* REAL, -20.0 *)

 u = 69-9;  (* INTEGER, 60 *)
 v = 10-20; (* INTEGER, -10 *)

 w = 50+20.0; (* REAL,70.0 *)
 x = -50+30.0; (* REAL, -20.0 *)

 y = 69+9;  (* INTEGER, 78 *)
 z = 10+(-20); (* INTEGER, -10 *)
 
 first="Mark";		(* STRING, "Mark" *)
 last='Chenoweth';  (* STRING, 'Chenoweth' *)

 ch1 = ' '; (* CHAR, ' ' *)
 ch2 = "M"; (* CHAR, "M" *)

 aa = 100/10; 	 (* REAL, 10.0 *)
 bb = 10*10;   	 (* INTEGER, 100 *)
 cc = 10.1 * 10; (* REAL, 101.0 *)
 dd = 100.0/5;   (* REAL, 20.0 *)

 ee = 100 DIV 25; (* INTEGER, 4 *)
 ff = 100 DIV 26; (* INTEGER, 3 *)
 gg = 100 MOD 26; (* INTEGER, 22 *)
 hh = 100 MOD 25; (* INTEGER, 0 *)

 pi = 3.141594; (* REAL, 3.141594 *)
 
 mm = ~0;
 nn = ~1;
 
 oo = ((-74-10)/32)*(23+17);   (* REAL, -105.0 *)
 pp = ((-74-10) MOD 32)*(23+17); (* INTEGER, -800 *)

 qq = gg+ff; (* INTEGER, 25 *)
 rr = gg/ee; (* REAL, 5.5 *)
 
 ss1 = "Mark"="Mark"; (* BOOLEAN, TRUE *)
 ss2 = "Mark"="xxx";  (* BOOLEAN, FALSE *)
 
 ss3 = "Mark"#"Mar";  (* BOOLEAN, TRUE *)
 ss4 = "Mark"#"Mark"; (* BOOLEAN, FALSE *)
 
 ss5 = "Mark"<"MarkCheno"; (* BOOLEAN, TRUE *)
 ss6 = "MarkCheno"<"Mark"; (* BOOLEAN, FALSE *)

 ss7 = "Mark"<="MarkCheno";         (* BOOLEAN, TRUE *)
 ss8 = "NMarkCheno"<="MarkChenoweth"; (* BOOLEAN, FALSE *)

 ss9  = "MarkCheno">"Mark"; (* BOOLEAN, TRUE *)
 ss10 = "Mark">"MarkCheno"; (* BOOLEAN, FALSE *)

 ss11 = "MarkCheno">="MarkCheno"; (* BOOLEAN, TRUE *)
 ss12 = "Mark">="MarkCheno"; (* BOOLEAN, FALSE *)
 


begin
end xxx.
