ifdef NEW_KERNEL
EXTRA_CFLAGS += -DNEW_KERNEL=$(NEW_KERNEL)
endif

ifdef DEBUG_ON
EXTRA_CFLAGS += -DDEBUG_ON=$(DEBUG_ON)
endif

ifdef MYMANET_ALPHA
EXTRA_CFLAGS += -DMYMANET_ALPHA=$(MYMANET_ALPHA)
endif

ifdef MYMANET_BETA
EXTRA_CFLAGS += -DMYMANET_BETA=$(MYMANET_BETA)
endif

ifdef MYMANET_KERNEL_VERSION_6_30_PLUS
EXTRA_CFLAGS += -DMYMANET_KERNEL_VERSION_6_30_PLUS=$(MYMANET_KERNEL_VERSION_6_30_PLUS)
endif

ifdef MYMANET_STORE_PATH
EXTRA_CFLAGS += -DMYMANET_STORE_PATH=$(MYMANET_STORE_PATH)
endif

obj-m += manifold.o
manifold-objs := main_module.o string_functions.o proc_functions.o distance_list_functions.o stat_list_functions.o timer_functions.o timestamp_list_functions.o path_list_functions.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.ko
	rm -f *.o
	rm -f *.mod.*
	rm -f Module.symvers
	rm -f Module.markers
	rm -f modules.order

