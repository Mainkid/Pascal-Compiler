program firstOne;
var a,b,d:integer;
r:real;
flag:boolean;
begin
a:=1;
b:=1;
while (b<100) do begin
writeln(b);
d:=b;
b:=a+b;
a:=d;
end
end.