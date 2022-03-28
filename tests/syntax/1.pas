PROGRAM OK;
var n,p1,p2,p3,p4:integer;
begin
p1:=n div 1000;
p4:=n mod 10;
p2:=(n div 100) mod 10;
p3:=(n div 10) mod 10;
if p1+p4=p2+p3 then p3:=1 else p1:=1;
end.