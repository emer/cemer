CD = cd

all: qmake
	@echo " "
	@echo "** Doing ($@) in all plugin directories**"
	@echo " "
	@for i in tacss/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) ) then echo "* Successfully ran make in $$i";\
	    else echo "*** Could not make $@ in $$i"; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@for i in pdp/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) ) then echo "* Successfully ran make in $$i";\
	    else echo "*** Could not make $@ in $$i"; fi;\
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
	@for i in tacss/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) $@; ) then echo "* Successfully made $@ in $$i";\
	    else echo "*** Could not make $@ in $$i"; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@for i in pdp/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( $(MAKE) $@; ) then echo "* Successfully made $@ in $$i";\
	    else  echo "*** Could not make $@ in $$i"; fi;\
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
	@for i in tacss/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( qmake; ) then echo "* Successfully ran $@ in $$i";\
	    else echo "*** Could not make $@ in $$i"; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@for i in pdp/* ;\
	do \
	  if [ -d $$i ] ;\
	  then \
	    echo $$i; \
	    $(CD) $$i;\
	    if ( qmake; ) then echo "* Successfully ran $@ in $$i";\
	    else echo "*** Could not make $@ in $$i"; fi;\
	    $(CD) ../;\
	  fi; \
	done
	@echo " "
	@echo "** Make $@ successfully completed! **"
	@echo " "

 
