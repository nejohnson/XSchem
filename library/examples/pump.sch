G {

process 
variable del: time := 0.4 ns;
variable delay: time;
variable last : time;
variable lowvalue: real;

begin
wait on ING , USC.cap;

last := now;

delay := del * USC.cap;
print("start pump process: " & pump'PATH_NAME & " " & time'image(now) );

if(now=0 ns) then
      USC <= RREAL_0;
elsif ING'event and ING='1' then
      USC.conduct<=conduct;
      transition(USC,val,delay);
elsif ING'event and ING='0' then
      USC.conduct<=conduct;
      transition(USC,0.0,delay);
elsif USC.cap'event and USC.cap > USC.cap'last_value and ING='1' then
      lowvalue := USC.value * USC.cap'last_value / USC.cap;
      glitch(USC, lowvalue, val, delay);
end if;

end process;



} 
V {} 
C {devices/iopin} 50 -90 0 0 {name=p1 lab=USC sig_type="rreal"} 
C {devices/ipin} -80 -90 0 0 {name=p2 lab=ING} 
C {devices/use} -150 -260 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;
use work.rrreal.all;

library work;
use work.rrreal.all;

} 
