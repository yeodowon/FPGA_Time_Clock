connect -url tcp:127.0.0.1:3121
targets -set -filter {jtag_cable_name =~ "Digilent Basys3 210183B31B5BA" && level==0 && jtag_device_ctx=="jsn-Basys3-210183B31B5BA-0362d093-0"}
fpga -file D:/project/fpga/StepMotor_Buzz_FND_vitis/StepMotor_Buzz_FND/_ide/bitstream/Design_StepMotor_Buzz_FND_wrapper.bit
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
loadhw -hw D:/project/fpga/StepMotor_Buzz_FND_vitis/Design_StepMotor_Buzz_FND_wrapper/export/Design_StepMotor_Buzz_FND_wrapper/hw/Design_StepMotor_Buzz_FND_wrapper.xsa -regs
configparams mdm-detect-bscan-mask 2
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
rst -system
after 3000
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
dow D:/project/fpga/StepMotor_Buzz_FND_vitis/StepMotor_Buzz_FND/Debug/StepMotor_Buzz_FND.elf
targets -set -nocase -filter {name =~ "*microblaze*#0" && bscan=="USER2" }
con
