G {

-- process
-- variable t : time;
-- variable C : rreal;
-- begin
--   wait on  A'transaction , B'transaction , ENAB'transaction until t /= now;
--   t := now;
-- 
--   A<=rreal'(0.0,0.0,0.0);
--   B<=rreal'(0.0,0.0,0.0);
--   for i in 1 to 6 loop
--     wait for 0 ns;
--   end loop;
-- 
--   if ENAB = '1' then
--     for i in 1 to 6 loop
--       C := resolved_real( (A, B) );
--       B <= C;
--       A <= C;
--       wait for 0 ns;
--     end loop;
--   end if;
-- 
-- end process;

process
  variable last : time;
  variable aa,bb : rreal;
begin

wait until ( SCHEDULE'transaction'event or A'event or B'event or ENAB'event ) and now>last;
last := now;
print(switch_rreal'PATH_NAME & " start switch process: " & time'image(now) );
if now= 0 ns then
  aa:=rreal'(0.0,0.0,0.0);
  bb:=rreal'(0.0,0.0,0.0);
  A <= aa ;
  B <= bb ;
elsif ENAB'event and ENAB='1' then SCHEDULE <= 4 after del;
elsif ENAB'event and ENAB='0' then SCHEDULE <= 3 after del;
elsif B'event  and ENAB='1' then SCHEDULE <= 2 after del;
elsif A'event  and ENAB='1' then SCHEDULE <= 1 after del;
elsif SCHEDULE'transaction'event and SCHEDULE=3 then -- ENAB=0 event
  print(switch_rreal'PATH_NAME & "     release outputs: " & time'image(now) );
  aa:=rreal'(0.0,0.0,0.0);
  bb:=rreal'(0.0,0.0,0.0);
  A <= aa ;
  B <= bb ;
elsif SCHEDULE'transaction'event  and SCHEDULE=4 then -- ENAB=1 event
  aa := B;
  bb := A;
  A <= aa ;
  print(switch_rreal'PATH_NAME & "     enab forcing A, done: " & time'image(now) );
  print2(A);
  B <= bb ;
  print(switch_rreal'PATH_NAME & "     enab forcing B, done: " & time'image(now) );
  print2(B);
elsif SCHEDULE'transaction'event and SCHEDULE=2 then -- B event
  print(switch_rreal'PATH_NAME & "     B'event: " & time'image(now) );
  aa.conduct := B.conduct - bb.conduct;
  aa.cap := B.cap - bb.cap;
  if aa.conduct /= 0.0 then
    aa.value := ( B.value * B.conduct - bb.value * bb.conduct) / aa.conduct;
  else
    aa.value := B.value;
  end if;
  A <= aa ;
elsif SCHEDULE'transaction'event  and SCHEDULE=1 then -- A event
  print(switch_rreal'PATH_NAME & "     A'event: " & time'image(now) );
  bb.conduct := A.conduct - aa.conduct;
  bb.cap := A.cap - aa.cap;
  if bb.conduct /= 0.0 then
    bb.value := ( A.value * A.conduct - aa.value * aa.conduct) / bb.conduct;
  else
    bb.value := A.value;
  end if;
  B <= bb ;
end if; -- now = 0
end process;
} 
V {} 
S {} 
C {use} 0 -310 0 0 {    library ieee,work;
    use ieee.std_logic_1164.all;
    use work.rrreal.all;
    use std.textio.all;
} 
C {ipin} 80 -120 0 0 {name=p3 lab=ENAB} 
C {iopin} 170 -40 0 0 {name=p1 lab=B sig_type=rreal} 
C {iopin} 40 -40 0 1 {name=p2 lab=A sig_type=rreal} 
C {arch_declarations} 20 -190 0 0 {

signal SCHEDULE : integer ;


procedure print2( signal x : in rreal)  is
begin
  print("       " & x'SIMPLE_NAME & ".value=" & real'image(x.value)& " t= " & time'image(now) );
  print("       " & x'SIMPLE_NAME & ".cap=" & real'image(x.cap) & " t= " & time'image(now));
  print("       " & x'SIMPLE_NAME & ".conduct=" & real'image(x.conduct) & " t= " & time'image(now));
end print2;
} 
