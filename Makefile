manual:
	pio run -t exec -e native -a $(SEED)
	python3 ./manual/generate_manual.py