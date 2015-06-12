################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../SSBlock.cpp \
../SSDateTime.cpp \
../SSDb.cpp \
../SSDBDriver.cpp \
../SSDBResult.cpp \
../SSField.cpp \
../SSLog.cpp \
../SSRList.cpp \
../SSSafeObject.cpp \
../SSString.cpp \
../SSThread.cpp \
../SSTuple.cpp \
../SSType.cpp \
../MySQLDriver.cpp \
../Atomic_gcc_x86.cpp \
../Log.cpp \
../SSTable.cpp \
../SSAlgorithm.cpp \

OBJS += \
./src/SSBlock.o \
./src/SSDateTime.o \
./src/SSDb.o \
./src/SSDBDriver.o \
./src/SSDBResult.o \
./src/SSField.o \
./src/SSLog.o \
./src/SSRList.o \
./src/SSSafeObject.o \
./src/SSString.o \
./src/SSThread.o \
./src/SSTuple.o \
./src/SSType.o \
./src/MySQLDriver.o \
./src/Atomic_gcc_x86.o \
./src/Log.o \
./src/SSTable.o\
./src/SSAlgorithm.o\

CPP_DEPS += \
./src/SSBlock.d \
./src/SSDateTime.d \
./src/SSDb.d \
./src/SSDBDriver.d \
./src/SSDBResult.d \
./src/SSField.d \
./src/SSLog.d \
./src/SSRList.d \
./src/SSSafeObject.d \
./src/SSString.d \
./src/SSThread.d \
./src/SSTuple.d \
./src/SSType.d \
./src/MySQLDriver.d \
./src/Atomic_gcc_x86.d \
./src/Log.d \
./src/SSTable.d \
./src/SSAlgorithm.d \

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../ -I../Driver `mysql_config --include` -DHAVE_GETTIMEOFDAY -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


