################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/subversion.cpp 

OBJS += \
./src/subversion.o 

CPP_DEPS += \
./src/subversion.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I/usr/include/apr-1.0 -I/usr/include/subversion-1 -I"/home/houman/workspace/eclipse-C++/subversion/kdesubversion" -I/usr/include/qt4/Qt -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


