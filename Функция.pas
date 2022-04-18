program firstOne;
var step,from,toVal,func:real;
begin
step:=0.1;
from:=0.5;
toVal:=1.5;
while (from<toVal) do begin
func:=2*from*from-from/2+4;
writeln(func);
from:=from+step;
end
end.