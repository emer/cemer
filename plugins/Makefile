CD = cd

all: 
	@echo " "
	@echo "** Doing ($@) in all plugin directories**"
	@echo " "
	@for i in * ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) ) then echo "* Successfully ran make in $$i";\
	    else exit 1; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@echo " "
	@echo "** Make $@ successfully completed! **"
	@echo " "

clean: 
	@echo " "
	@echo "** Doing ($@) in all plugin directories**"
	@echo " "
	@for i in * ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) $@; ) then echo "* Successfully made $@ in $$i";\
	    else exit 1; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@echo " "
	@echo "** Make $@ successfully completed! **"
	@echo " "

qmake: 
	@echo " "
	@echo "** Doing ($@) in all plugin directories**"
	@echo " "
	@for i in * ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( qmake; ) then echo "* Successfully ran $@ in $$i";\
	    else exit 1; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@echo " "
	@echo "** Make $@ successfully completed! **"
	@echo " "

 
