default:
	make -sf Makefilecat mycat
	make -sf Makefilecp mycp
	make -sf Makefilels myls
	make -sf Makefilesh

clean:
	make -sf Makefilecat clean
	make -sf Makefilecp clean
	make -sf Makefilels clean
	make -sf Makefilesh clean
