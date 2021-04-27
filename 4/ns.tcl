
# Process (basically functions)
# Routine for finish
proc finish {} {
  global ns nf throughput1 throughput2 timer tracefile tr tr1 tr2
  
  $ns flush-trace
  close $nf
  close $tracefile
  close $tr

  puts "Average Throughput (SRC1) : [expr $throughput1/($timer-200)] Mbps"
  puts "Average Throughput (SRC2) :[expr $throughput2/($timer-200)] Mbps"
  puts "Ratio = [expr $throughput1/($throughput2)]"

  #plot using python
  
  # puts "RUNNIG NAM"
  # exec nam out_nam
  puts "Plotting Throughput"
  # uncomment for submission
  # exec xgraph tr1.tr -geometry 800x400  &
  # exec xgraph tr2.tr -geometry 800x400  &
  exec python3 plot.py &
  exit 0
}

#Routine to record simulation
proc sim {} {
  global tr throughput1 throughput2 sink1 sink2 timer start tr1 tr2
  set ns [Simulator instance]
  
  set step 0.5
  set time_now [$ns now]
  set bw1 [$sink1 set bytes_]
  set bw2 [$sink2 set bytes_]

  set new_th1 [expr $bw1/$step*8/1000000]
  set new_th2 [expr $bw2/$step*8/1000000]
  
  #saving to txt file for python plot
  puts $tr "$time_now,$new_th1,$new_th2"
  #uncomment for submission
  # puts $tr1 "$time_now $new_th1"
  # puts $tr2 "$time_now $new_th2"
  
  if { $time_now >= 100 } {
	set throughput1 [expr $throughput1+$new_th1 ]
	set throughput2 [expr $throughput2+$new_th2 ]
        }
  set timer [expr $timer+1] 
  $sink1 set bytes_ 0
  $sink2 set bytes_ 0

  #running recursively for 400 seconds with output recorded at every 0.5 seconds
  $ns at [expr $time_now+$step] "sim"
}

#check arguments
if { $argc != 2 } {
  puts "ns ns.tcl <TCP_tcp_flavor> <case_number>"
  puts "TCP_TYPE : Vegas or Sack"
  puts "CASE_NUM : 1,2 or 3"
}


set tcp_type [lindex $argv 0]
set case [lindex $argv 1]
set start 0
set stop_time 400

switch $case {
  global delay
  1 {set delay "12.5ms"}
  2 {set delay "20.0ms"}
  3 {set delay "27.5ms"}
}


#Simulator
set ns [new Simulator]
set tracefile [open out.tr w]
$ns trace-all $tracefile

# Network Assignments and connections
set src1 [$ns node]
set src2 [$ns node]
set rcv1 [$ns node]
set rcv2 [$ns node]
set r1 [$ns node]
set r2 [$ns node]

ns color 1 Blue
ns color 2 Red

$ns duplex-link $src1 $r1 10Mb 5ms DropTail
$ns duplex-link $src2 $r1 10Mb $delay DropTail
$ns duplex-link $r1 $r2 1Mb 5ms DropTail
$ns duplex-link $rcv1 $r2 10Mb 5ms DropTail
$ns duplex-link $rcv2 $r2 10Mb $delay DropTail

#TCP Source and Sink with FTP
set tcp1 [new Agent/TCP/$tcp_type]
set tcp2 [new Agent/TCP/$tcp_type]
$ns attach-agent $src1 $tcp1
$ns attach-agent $src2 $tcp2
    
$tcp1 set class_ 1
$tcp2 set class_ 2


set sink1 [new Agent/TCPSink]
set sink2 [new Agent/TCPSink]
$ns attach-agent $rcv1 $sink1
$ns attach-agent $rcv2 $sink2
    
$ns connect $tcp1 $sink1
$ns connect $tcp2 $sink2

set ftp1 [new Application/FTP]
set ftp2 [new Application/FTP]
    
$ftp1 attach-agent $tcp1
$ftp2 attach-agent $tcp2

#NAM 
set nf [open out_nam.nam w] 
$ns namtrace-all $nf

$ns duplex-link-op $src1 $r1 orient right-down
$ns duplex-link-op $src2 $r1 orient right-up
$ns duplex-link-op $r1 $r2 orient right
$ns duplex-link-op $rcv1 $r2 orient left-down
$ns duplex-link-op $rcv2 $r2 orient left-up

set throughput1 0
set throughput2 0
set timer 0
#xgraph not available using txt file to plot through python
set tr [open tr.csv w]
puts $tr "time,th1,th2"

# Uncomment for final commit 
# set tr1 [open tr1.tr w]
# set tr2 [open tr2.tr w]

#Setting up the simulation
$ns at $start "sim"
$ns at $start "$ftp1 start"
$ns at $start "$ftp2 start"

$ns at $stop_time "$ftp1 stop"
$ns at $stop_time "$ftp2 stop"
$ns at $stop_time "finish"

set tf1 [open "$tcp_type $case-out1.tr" w]
$ns trace-queue  $src1  $r1  $tf1
    
set tf2 [open "$tcp_type $case-out2.tr" w]
$ns trace-queue  $src2  $r1  $tf2

$ns run
