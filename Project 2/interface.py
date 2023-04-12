

import PySimpleGUI as sg
from explain import *


class GUI:
    
    def __init__(self):
        return
    
    # def initialise_GUI(self):
        
    #     font = 'Consolas'
        
    #     col1 = [
    #         [sg.Text('Host:', font=(font, 12), justification='left')],
    #         [sg.Text('Port:', font=(font, 12), justification='left')],
    #         [sg.Text('Database:', font=(font, 12), justification='left')],
    #         [sg.Text('Username:', font=(font, 12), justification='left')],
    #         [sg.Text('Password:', font=(font, 12), justification='left')],
    #     ]

    #     col2 = [
    #         [sg.Input(key='host', font=(font, 12), default_text='localhost')],
    #         [sg.Input(key='port', font=(font, 12), default_text='5432')],
    #         [sg.Input(key='database', font=(font, 12), default_text='TPC-H')],
    #         [sg.Input(key='username', font=(font, 12), default_text='postgres')],
    #         [sg.InputText('', key='password', password_char='*', font=(font, 12))],
    #     ]
        
    #     layout = [  
    #         [sg.Text('Query Execution Plan Annotator', key='-text-', font=(font, 20))],
    #         [sg.Frame(layout=col1, title=''), sg.Frame(layout=col2, title='')],
    #         [sg.Button('Submit')]
    #     ]
        
        
    #     # Create the Window
    #     window = sg.Window('CX4031 Project 2 GUI', layout, element_justification='c', return_keyboard_events=True).Finalize()
    #     window.bind("<Return>", "_Enter")
        
    #     while True:
    #         event, values = window.read()
    #         if event == 'Submit':   # when user clicks submit button

    #             # get all inputs
    #             host = values['host'].lower()   #localhost
    #             port = values['port']   #5432
    #             database = values['database']   #whatever ur database name is
    #             username = values['username'].lower()   #postgres
    #             password = values['password']   #whatever ur password is
    #             window.close()
    #             return host, port, database, username, password

    #         elif event == "_Enter":
    #             # get all inputs
    #             host = values['host'].lower()   #localhost
    #             port = values['port']   #5432
    #             database = values['database']   #whatever ur database name is
    #             username = values['username'].lower()   #postgres
    #             password = values['password']   #whatever ur password is
    #             window.close()
    #             return host, port, database, username, password

    #         if event == sg.WIN_CLOSED: # if user closes window
    #             break

    #     window.close()
        
    def main_window(self,connect):
        
        self.apqs = [True, True, True, True, True, True, True, True, True, True, True, True]
        self.connect = connect
        root = tk.Tk()
        root.title("Query Visualiser")
        root.state('zoomed')
        def close():
            root.destroy()
            return 
        
        # frame = tk.Frame(root, width = 1000, height = 1200)
        # frame.pack(expand=True, fill="both")

        # canvas_frame = tk.Frame(frame)
        # canvas_frame.pack(side="left", fill="both", expand=True)
        
        
        # # create file menu to exit 
        # menu = tk.Menu(root)
        # root.config(menu = menu)
        # filemenu = tk.Menu(menu)
        # menu.add_cascade(label = "File" , menu = filemenu)
        # filemenu.add_command(label = "Exit" , command = close , accelerator="Ctrl+Q")
        # root.bind_all("<Control-q>" , lambda e : close())
        # root.mainloop()
        
        
        
        
        
        


    