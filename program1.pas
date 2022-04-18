program firstOne;
var a,b,d:integer;
r:real;
flag:boolean;
begin
a:=42;
b:=35;
while (a <> b) do begin
if (a>b) then
a:=a-b
else
b:=b-a;
end;
writeln(a)
end.