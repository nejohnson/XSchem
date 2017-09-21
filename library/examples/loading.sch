G {
-- these assignments are done to have the voltage values available
-- in the waveform file
V_VX <= VX.value;
V_VX2 <= VX2.value;
V_VXS <= VXS.value;
V_SP <= SP.value;


process
begin
ING<='0';
ING1<='0';
SW <= '0';
SW1<='0';
SW2<='0';
--VX <= rreal'(4.5,10.0,0.0);
--VX2 <= rreal'(3.0, 5.0, 0.0);
wait for 200 ns;
ING1<='1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW<='1';
wait for 200 ns;
SW2<='1';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW2<='0';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW<='1';
wait for 200 ns;
ING <='0';
wait for 200 ns;
SW1<= '0';
wait for 200 ns;
SW<='1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW <= '0';
wait for 200 ns;
SW1<= '1';
wait for 200 ns;
ING<='1';
wait for 200 ns;
SW<='0';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW<='1';
wait for 200 ns;
SW1<='1';
wait for 200 ns;
SW1<='0';
wait for 200 ns;
SW1<='1';
wait for 200 ns;



wait;
end process;

} 
V {// test} 
S {* test} 
T {rrreal type:
-----------

rreal is a record type containing voltage value, drive strength and
capacitive loading of an electrical node.

rrreal is a resolved subtype of rreal.

The resolution function invoked by the simulator updates
voltages, strengths and capacitive loading of all nodes.

this allows to simulate voltage transients, charge sharing,
floating conditions and more.
the example uses bidirectional analog switches and simulates charge pumps 
which have a finite driving capability (output impedance)} 10 -410 0 0 0.3 0.3 {} 
T {VHDL DESIGN EXAMPLE} 140 -1290 0 0 1 1 {} 
T {set netlist mode to VHDL
- create netlist
- simulate with ghdl
- view waveforms} 110 -1200 0 0 0.6 0.6 {} 
N 830 -680 900 -680 {lab=VXS} 
N 450 -680 510 -680 {lab=VX} 
N 450 -680 450 -570 {lab=VX} 
N 1230 -680 1240 -680 {lab=SP} 
N 340 -680 450 -680 {lab=VX} 
N 830 -680 830 -570 {lab=VXS} 
N 1230 -680 1230 -570 {lab=SP} 
N 470 -800 480 -800 {lab=VX2} 
N 810 -800 810 -680 {lab=VXS} 
N 780 -800 810 -800 {lab=VXS} 
N 350 -910 470 -910 {lab=VX2} 
N 470 -910 470 -800 {lab=VX2} 
N 810 -680 830 -680 {lab=VXS} 
N 1200 -680 1230 -680 {lab=SP} 
N 340 -800 470 -800 {lab=VX2} 
C {devices/lab_wire} 430 -680 0 1 {name=l1  lab=VX sig_type=rrreal } 
C {devices/use} 680 -310 0 0 {library ieee;
use std.TEXTIO.all;
use ieee.std_logic_1164.all;

library work;
use work.rrreal.all;

} 
C {examples/pump} 250 -680 0 0 {name=x4 conduct="1.0/20000.0" val=4.5} 
C {devices/lab_pin} 150 -680 0 0 {name=l4  lab=ING} 
C {examples/switch_rreal} 660 -670 0 0 {name=x5 del="2 ns"} 
C {devices/lab_pin} 510 -660 0 0 {name=l5  lab=SW} 
C {examples/real_capa} 450 -540 0 0 {name=x3 cap=30.0} 
C {examples/real_capa} 830 -540 0 0 {name=x1 cap=100.0} 
C {examples/switch_rreal} 1050 -670 0 0 {name=x2 del="2 ns"} 
C {devices/lab_pin} 900 -660 0 0 {name=l2  lab=SW1} 
C {devices/lab_pin} 1240 -680 0 1 {name=l3  lab=SP sig_type=rrreal} 
C {examples/real_capa} 1230 -540 0 0 {name=x6 cap=20.0} 
C {devices/lab_wire} 860 -680 0 1 {name=l6  lab=VXS sig_type=rrreal
} 
C {examples/pump} 250 -800 0 0 {name=x7 conduct="1.0/40000.0" val=3.0} 
C {devices/lab_pin} 150 -800 0 0 {name=l7  lab=ING1} 
C {examples/switch_rreal} 630 -790 0 0 {name=x8 del="2 ns"} 
C {devices/lab_pin} 480 -780 0 0 {name=l0  lab=SW2} 
C {devices/lab_wire} 460 -800 0 1 {name=l8  lab=VX2 sig_type=rrreal
} 
C {examples/real_capa} 350 -880 0 0 {name=x9 cap=40.0} 
C {devices/package_not_shown} 670 -430 0 0 {
    library ieee, std;
    use std.textio.all;
 
    package rrreal is



    type rreal is
    record
      value : real;
      conduct : real;
      cap : real;
    end record;



    type rreal_vector is array (natural range <>) of rreal;
    function resolved_real( r: rreal_vector ) return rreal; 
    procedure print(s : in string);
    subtype rrreal is resolved_real rreal;
    type rrreal_vector is array (natural range <>) of rrreal;

    CONSTANT RREAL_X : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>-1.0);
    CONSTANT RREAL_Z : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>0.0);
    CONSTANT RREAL_0 : rreal := rreal'(value=> 0.0, cap=>0.0, conduct=>10.0);
    CONSTANT REAL_Z : real := 20.0;
    CONSTANT REAL_X : real := 20.0;


    procedure transition(
        signal sig: INOUT rreal; 
        constant endval: IN real; 
        constant del: IN time
    );

    procedure glitch(
        signal sig: INOUT rreal;
        constant lowval: IN real;
        constant endval: IN real;
        constant del: IN time
    );

    end rrreal; -- end package declaration
 
 
    package body rrreal is

procedure print(s : in string) is
variable outbuf: line;
begin
  write(outbuf, s);
  writeline(output, outbuf);
end procedure;

 
--      function resolved_real( r:rreal_vector) return rreal is
--      VARIABLE result : rreal := RREAL_Z;
--      begin
--        IF    (r'LENGTH = 1) THEN    RETURN r(r'LOW);
--        ELSE
--            FOR i IN r'RANGE LOOP
--              result.cap := result.cap + r(i).cap ;
--              IF r(i).value /=REAL_Z THEN
--                IF result.value /=REAL_Z THEN 
--                  result.value := REAL_X ;
--                ELSE
--                  result.value := r(i).value ;
--                END IF;
--              END IF ;
--            END LOOP;
--        END IF;
--        RETURN result;
--      end resolved_real;
 
    function resolved_real( r:rreal_vector) return rreal is
    VARIABLE result : rreal := RREAL_Z;
    variable vcapshare : real := 0.0;
    begin
      IF    (r'LENGTH = 1) THEN    RETURN r(r'LOW);
      ELSE
          FOR i IN r'RANGE LOOP
            if r(i).conduct = -1.0 then
              result := RREAL_X;
              exit;
            end if;
           
            -- only process initialized (valid) data
            if r(i).value > -30.0 and r(i).value < 30.0 then
              if r(i).cap  > -1.0e12 and r(i).cap  < 1.0e12 then
                if r(i).conduct  > -1.0e12 and r(i).conduct  < 1.0e12 then
                  vcapshare := vcapshare + r(i).value * r(i).cap;
                  result.value := result.value + r(i).value *  r(i).conduct;
                  result.cap := result.cap + r(i).cap ;
                  if(r(i).conduct > 0.0 ) then
                    -- result.conduct := result.conduct + 1.0/r(i).conduct ;
                    result.conduct := result.conduct + r(i).conduct ;
                  end if;
                end if;
              end if;
            end if;

          END LOOP;
      END IF;

      if result.conduct /= 0.0 then 
        result.value := result.value / result.conduct ;     -- conductance
        -- result.value := result.value * result.conduct ;     -- resistance
        -- result.conduct := 1.0 / result.conduct;
      elsif result.cap >0.0 then
        result.value := vcapshare / result.cap;
      else 
        result.value:=0.0;
      end if;

      RETURN result;
    end resolved_real;

    procedure transition(
        signal sig: INOUT rreal; 
        constant endval: IN real; 
        constant del: IN time) is

        variable step: real;
        variable startval: real;
        variable del2: time;
    begin
     del2 := del;
     if del2 = 0 fs then
       del2 := 1 ns;
     end if;
     startval := sig.value;
     step := (endval-startval);
     if abs(endval-startval) < 0.01 then --do not propagate events if endval very close to startval
       return;
     end if;
--       sig.value <= endval after del;
        sig.value <= startval,
                     startval+0.25*step after del2*0.1,
                     startval+0.45*step after del2*0.2,
                     startval+0.60*step after del2*0.3,
                     startval+0.72*step after del2*0.4,
                     startval+0.80*step after del2*0.5,
                     startval+0.86*step after del2*0.6,
                     startval+0.90*step after del2*0.7,
                     startval+0.94*step after del2*0.8,
                     startval+0.97*step after del2*0.9,
                     endval             after del2;
    end transition;


    procedure glitch(
        signal sig: INOUT rreal;
        constant lowval: IN real;
        constant endval: IN real;
        constant del: IN time) is

        variable step: real;
        variable step2: real;
        variable startval: real;
        variable del2 : time;
    begin
     del2 := del;
     if del2 = 0 fs then
       del2 := 1 ns;
     end if;
     startval := sig.value;
     step := (lowval-startval);
     step2 := (lowval-endval);
     if abs(lowval-startval) < 0.01 then --do not propagate events if endval very close to startval
       return;
     end if;
        sig.value <= 
--                     startval,
--                     startval+0.25*step after del*0.05,
--                     startval+0.45*step after del*0.1,
--                     startval+0.60*step after del*0.15,
--                     startval+0.72*step after del*0.2,
--                     startval+0.80*step after del*0.25,
--                     startval+0.86*step after del*0.3,
--                     startval+0.90*step after del*0.35,
--                     startval+0.94*step after del*0.4,
--                     startval+0.97*step after del*0.45,
--                     lowval after del*0.5,
--                     lowval-0.25*step2 after del*0.55,
--                     lowval-0.45*step2 after del*0.6,
--                     lowval-0.60*step2 after del*0.65,
--                     lowval-0.72*step2 after del*0.7,
--                     lowval-0.80*step2 after del*0.75,
--                     lowval-0.86*step2 after del*0.8,
--                     lowval-0.90*step2 after del*0.85,
--                     lowval-0.94*step2 after del*0.9,
--                     lowval-0.97*step2 after del*0.95,
--                     endval after del;
                     lowval,
                     lowval-0.25*step2 after del2*0.1,
                     lowval-0.45*step2 after del2*0.2,
                     lowval-0.60*step2 after del2*0.3,
                     lowval-0.72*step2 after del2*0.4,
                     lowval-0.80*step2 after del2*0.5,
                     lowval-0.86*step2 after del2*0.6,
                     lowval-0.90*step2 after del2*0.7,
                     lowval-0.94*step2 after del2*0.8,
                     lowval-0.97*step2 after del2*0.9,
                     endval after del2;


    end glitch;


    end rrreal; -- end package body
} 
C {devices/title} 160 0 0 0 {name=l9 author="Stefan Schippers"} 
C {devices/arch_declarations} 670 -370 0 0 {
signal V_VX, V_VX2, V_VXS, V_SP: real;

} 