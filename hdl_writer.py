import subprocess

def hierarquical(circuit):
	
    # Calling C++ Nodes to find the Cluster Roots
	
    # Executable file of the C++ code
	exe_str = r".\cluster_points.exe"

	
	itr_file = r".\\Circuits\\ "[:-1] + circuit + ".v" 
	process_data = subprocess.run([exe_str, itr_file], timeout=None, stdout=subprocess.PIPE, shell=True)
	print(process_data.stdout) # To check what the subprocess is sending
	cluster_roots = process_data.stdout[20:].split()
	print(cluster_roots)

    # Reading the original verilog file 

	netlist = open(r".\Circuits\ "[:-1] + circuit + ".v", "r")

	text = netlist.readlines()

	line = 0
	modules, voteds = [], []
	new_text = ''

	while line < len(text):
		fl = text[line]

		if fl[:2] == "//" or fl[:2] == "/*" or fl.isspace():
			new_fl = fl
		else:
			fl_white_spaces = fl.split()

			tipo = fl_white_spaces[0]

			if tipo == "module" or tipo == "endmodule":
				new_fl = fl

			# Inputs and outputs
			elif tipo.find("input") != -1 or tipo.find("output") != -1 or tipo.find("wire") != -1:
				find = fl.find(fl_white_spaces[1]) # Find the beginning of the elements
				ports = fl[find:-2].split(",")
				# For multiple lines
				while (not fl[len(fl) - 2] == ";"):
					line += 1
					fl = text[line]
					ports += fl[fl.find(' '):-2].split(",")
				
				if tipo.find("input") != -1:
					new_fl = "input " + ''.join(map(lambda x: x + ',', ports))[:-2] + ';'
					inputs = ports
				elif tipo.find("wire") != -1:
					wires = ports
					new_fl = "wire " + ''.join(map(lambda x: x + '_1,', ports)) + '\n' + ''.join(map(lambda x: x + '_2,', ports)) + '\n' + ''.join(map(lambda x: x + '_3,', ports))[:-2] + ';'
				else:
					new_fl = "output " + ''.join(map(lambda x: x + ',', ports))[:-2] + ';'
					outputs = ports
			
			# Gates
			else: 
				logic_gate = tipo
				gate_name = fl_white_spaces[1]
				ports = fl[fl.find("(") + 1:-3].split(", ")
				n_entradas = len(ports) - 1

				new_ports1 = list(map(lambda x: x + '_1'))
				new_ports2 = list(map(lambda x: x + '_2'))
				new_ports3 = list(map(lambda x: x + '_3'))
				
				# Formação do log
				data = []
				input_locations = []
				voted = []

				for i in range(len(ports)):
					if ports[i] in inputs:
						input_locations += i
					if ports[i] in voteds:
						voted += i

				if len(input_locations) != 0:
					for input_location in input_locations:
						new_ports1[input_location] = new_ports1[input_location][:-3]

				if ports[0] in cluster_roots:
					# Call Voter


					voteds += ports[0]
					vote = True
				
				if vote:
					gate = voteds[len(voteds) - 1]
					n_voter = len(voteds)

					new_fl += 'and AND2_' + str(n_voter) + '(Q' + str(n_voter) + '_1, ' + gate + '_1, ' + gate + '_2);\n'
					new_fl += 'and AND2_' + str(n_voter + 1) + '(Q' + str(n_voter) + '_2, ' + gate + '_1, ' + gate + '_3);\n'
					new_fl += 'and AND2_' + str(n_voter + 2) + '(Q' + str(n_voter) + '_3, ' + gate + '_2, ' + gate + '_3);\n'
					new_fl += 'or OR3_' + str(n_voter) + '(' + gate + '_V, Q' + str(n_voter) + '_1, Q' + str(n_voter) + '_2, Q' + str(n_voter) + '_3);\n'

				data_log = [data[0]] + [n_entradas] + data[1:] + tabela_verdade
				
				log_text += [list(map(lambda x:str(x) + " ", data_log))]

		line += 1
	print(text)

def elements_quantity(elements):
	elements_len = 0
						
	for element in elements:
		elements_len += int(element[1])

	return elements_len

def truncate(n):
	return int(n * 10000) / 10000