################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app_cmd_analysis.c \
../app_cmd_exe.c \
../app_grab_cfg.c \
../app_storage_cfg.c \
../cyfxslfifosync.c \
../cyfxslfifousbdscr.c \
../cyfxtx.c \
../fpga_config.c \
../fx3_common.c \
../fx3_fifo.c \
../fx3_pin_init.c \
../fx3_spi.c \
../mcu_spi.c 

S_UPPER_SRCS += \
../cyfx_gcc_startup.S 

OBJS += \
./app_cmd_analysis.o \
./app_cmd_exe.o \
./app_grab_cfg.o \
./app_storage_cfg.o \
./cyfx_gcc_startup.o \
./cyfxslfifosync.o \
./cyfxslfifousbdscr.o \
./cyfxtx.o \
./fpga_config.o \
./fx3_common.o \
./fx3_fifo.o \
./fx3_pin_init.o \
./fx3_spi.o \
./mcu_spi.o 

C_DEPS += \
./app_cmd_analysis.d \
./app_cmd_exe.d \
./app_grab_cfg.d \
./app_storage_cfg.d \
./cyfxslfifosync.d \
./cyfxslfifousbdscr.d \
./cyfxtx.d \
./fpga_config.d \
./fx3_common.d \
./fx3_fifo.d \
./fx3_pin_init.d \
./fx3_spi.d \
./mcu_spi.d 

S_UPPER_DEPS += \
./cyfx_gcc_startup.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -marm -mthumb-interwork -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -D__CYU3P_TX__=1 -I"C:\Program Files (x86)\Cypress\EZ-USB FX3 SDK\1.3\/fw_lib/1_3_3/inc" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=arm926ej-s -marm -mthumb-interwork -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -x assembler-with-cpp -I"C:\Program Files (x86)\Cypress\EZ-USB FX3 SDK\1.3\/fw_lib/1_3_3/inc" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


