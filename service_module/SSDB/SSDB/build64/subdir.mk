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
../SSFunctional.cpp \
../SSHead.cpp \
../SSMyLog.cpp \
../SSRList.cpp \
../SSSafeObject.cpp \
../SSString.cpp \
../SSThread.cpp \
../SSTuple.cpp \
../SSType.cpp \
../MySQLDriver.cpp \
../ODBCDriver.cpp \
../Atomic_gcc_x86.cpp \
../SSLog.cpp \
../SSTable.cpp \
../SSAlgorithm.cpp \
../SSSQLCreator.cpp \
../SSSQLCreator_Mysql.cpp \
../SSSQLCreator_Oracle.cpp \
../oci_Data.cpp \
../OCIDriver.cpp \
../oci_HandleMng.cpp \

OBJS += \
./src/SSBlock.o \
./src/SSDateTime.o \
./src/SSDb.o \
./src/SSDBDriver.o \
./src/SSDBResult.o \
./src/SSField.o \
./src/SSFunctional.o \
./src/SSHead.o \
./src/SSMyLog.o \
./src/SSRList.o \
./src/SSSafeObject.o \
./src/SSString.o \
./src/SSThread.o \
./src/SSTuple.o \
./src/SSType.o \
./src/MySQLDriver.o \
./src/ODBCDriver.o \
./src/Atomic_gcc_x86.o \
./src/SSLog.o \
./src/SSTable.o\
./src/SSAlgorithm.o\
./src/SSSQLCreator.o \
./src/SSSQLCreator_Mysql.o \
./src/SSSQLCreator_Oracle.o \
./src/oci_Data.o \
./src/OCIDriver.o \
./src/oci_HandleMng.o \

CPP_DEPS += \
./src/SSBlock.d \
./src/SSDateTime.d \
./src/SSDb.d \
./src/SSDBDriver.d \
./src/SSDBResult.d \
./src/SSField.d \
./src/SSFunctional.d \
./src/SSHead.d \
./src/SSMyLog.d \
./src/SSRList.d \
./src/SSSafeObject.d \
./src/SSString.d \
./src/SSThread.d \
./src/SSTuple.d \
./src/SSType.d \
./src/MySQLDriver.d \
./src/ODBCDriver.d \
./src/Atomic_gcc_x86.d \
./src/SSLog.d \
./src/SSTable.d \
./src/SSAlgorithm.d \
./src/SSSQLCreator.d \
./src/SSSQLCreator_Mysql.d \
./src/SSSQLCreator_Oracle.d \
./src/oci_Data.d \
./src/OCIDriver.d \
./src/oci_HandleMng.d \

# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -I../ -I/home/wuxiaoxing/codebase/Dist/include -I../Driver -I../oci -I../oci/include -I/opt/dist/include/mysql -DMYSQL_DRIVER -DOCI_DRIVER -DHAVE_GETTIMEOFDAY -DLINUX -O3 -Wall -fPIC -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


