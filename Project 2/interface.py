import tkinter as tk
import PySimpleGUI as sg
import explain as ex

class GUI:
    
    def __init__(self):
        return
    
    def initialise_GUI(self):
        
        font = 'Consolas'
        
        col1 = [
            [sg.Text('Host:', font=(font, 12), justification='left')],
            [sg.Text('Port:', font=(font, 12), justification='left')],
            [sg.Text('Database:', font=(font, 12), justification='left')],
            [sg.Text('Username:', font=(font, 12), justification='left')],
            [sg.Text('Password:', font=(font, 12), justification='left')],
        ]

        col2 = [
            [sg.Input(key='host', font=(font, 12), default_text='localhost')],
            [sg.Input(key='port', font=(font, 12), default_text='5432')],
            [sg.Input(key='database', font=(font, 12), default_text='TPC-H')],
            [sg.Input(key='username', font=(font, 12), default_text='postgres')],
            [sg.InputText('', key='password', password_char='*', font=(font, 12))],
        ]
        
        layout = [  
            [sg.Text('Query Execution Plan Annotator', key='-text-', font=(font, 20))],
            [sg.Frame(layout=col1, title=''), sg.Frame(layout=col2, title='')],
            [sg.Button('Submit')]
        ]
        
        
        # Create the Window
        window = sg.Window('CX4031 Project 2 GUI', layout, element_justification='c', return_keyboard_events=True).Finalize()
        window.bind("<Return>", "_Enter")
        
        while True:
            event, values = window.read()
            if event == 'Submit':   # when user clicks submit button

                # get all inputs
                host = values['host'].lower()   #localhost
                port = values['port']   #5432
                database = values['database']   #whatever ur database name is
                username = values['username'].lower()   #postgres
                password = values['password']   #whatever ur password is
                window.close()
                return host, port, database, username, password

            elif event == "_Enter":
                # get all inputs
                host = values['host'].lower()   #localhost
                port = values['port']   #5432
                database = values['database']   #whatever ur database name is
                username = values['username'].lower()   #postgres
                password = values['password']   #whatever ur password is
                window.close()
                return host, port, database, username, password

            if event == sg.WIN_CLOSED: # if user closes window
                break

        window.close()

    def structure_query(self,query):
        new_text = ''
        for text in query.lower().split(' '):
            if text == 'from' or  text == "where":
                new_text += '\n' + text + " "
            elif text == 'and' or  text == "or":
                new_text += '\n\t' + text + " "
            else:
                new_text += text + " "
        query = new_text
        return query

    def onScroll_json(self, event):
        self.query_json_canvas.yview_scroll(int(-1*(event.delta/120)), "units")
          
    def main_window(self,connect):
        

        self.apqs1 = [True, True, True, True, True, True, True, True, True, True, True, True]
        self.apqs2 = [True, True, True, True, True, True, True, True, True, True, True, True]
        
        self.connect = connect
        root = tk.Tk()
        self.root = root
        root.title("Query Visualiser")
        root.state('normal')

        def close():
            root.destroy()
            return 


        label_q1 = tk.Label(root, text="Query 1", font=("Helvetica", 18))
        label_q1.pack()
        q1_frame = tk.Canvas(root, width = 1000, height = 1200)
        q1_frame.pack(expand=True, fill="both")

        canvas_frame = tk.Canvas(q1_frame)
        canvas_frame.pack(side="left", fill="both", expand=True)

        #query text
        # Query 1--------------------------------------------------

        self.query_text_canvas = tk.Canvas(q1_frame, bg = 'white')

        query_text_scrollbar_v = tk.Scrollbar(q1_frame, orient = tk.VERTICAL)
        query_text_scrollbar_v.place(relx=0.49, rely=0.5, relwidth=0.01, relheight=0.5)
        query_text_scrollbar_v.config(command=self.query_text_canvas.yview)

        query_text_scrollbar_h = tk.Scrollbar(q1_frame, orient = tk.HORIZONTAL)
        query_text_scrollbar_h.place(relx=0, rely=0.98, relwidth=0.5, relheight=0.02)
        query_text_scrollbar_h.config(command=self.query_text_canvas.xview)

        self.query_text_canvas.config(yscrollcommand = query_text_scrollbar_v.set, xscrollcommand = query_text_scrollbar_h.set)
        self.query_text_canvas.bind('<Configure>', lambda e: self.query_text_canvas.configure(scrollregion=self.query_text_canvas.bbox("all")))
        self.query_text_canvas.place(relx=0, rely=0.5, relheight=0.48, relwidth=0.49)
        self.query_text_canvas.create_text(250, 70, text='', tags='querytext')

        #query plan canvas (Draw the graph)-----------------------
        self.canvas = tk.Canvas(canvas_frame, bg='white', bd=2)

        scrollbar_v = tk.Scrollbar(canvas_frame, orient = tk.VERTICAL, bg='blue')
        scrollbar_v.place(relx=0.99, rely=0, relheight=0.5, relwidth=0.01)
        scrollbar_v.config(command=self.canvas.yview)
        self.canvas.config(yscrollcommand=scrollbar_v.set)

        scrollbar_h = tk.Scrollbar(canvas_frame, orient = tk.HORIZONTAL, bg='red')
        scrollbar_h.place(relx=0, rely=0.48, relheight=0.02, relwidth=0.99)
        scrollbar_h.config(command=self.canvas.xview)
        self.canvas.config(xscrollcommand=scrollbar_h.set)

        self.canvas.bind('<Configure>', lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all")))
        self.canvas.place(relx=0, rely=0, relheight=0.48, relwidth=0.99)

 

        self.query_json_canvas = tk.Canvas(q1_frame, bg = 'white')
        query_json_scrollbar_v = tk.Scrollbar(q1_frame, orient = tk.VERTICAL)
        query_json_scrollbar_v.place(relx=0.99, rely=0.5, relwidth=0.01, relheight=0.5)
        query_json_scrollbar_v.config(command=self.query_json_canvas.yview)

        query_json_scrollbar_h = tk.Scrollbar(q1_frame, orient = tk.HORIZONTAL)
        query_json_scrollbar_h.place(relx=0.5, rely=0.98, relwidth=0.5, relheight=0.02)
        query_json_scrollbar_h.config(command=self.query_json_canvas.xview)

        self.query_json_canvas.config(xscrollcommand=query_json_scrollbar_h.set, yscrollcommand=query_json_scrollbar_v.set)
        self.query_json_canvas.bind('<Configure>', lambda e: self.query_json_canvas.configure(scrollregion=self.query_json_canvas.bbox("all")))
        self.query_json_canvas.place(relx=0.5, rely=0.5, relheight=0.48, relwidth=0.49)
        self.query_json_canvas.create_text(250, 70, text = '', tags="jsonquery")
        self.query_json_canvas.bind_all("<MouseWheel>" , self.onScroll_json)
        
        
        
        label_q2 = tk.Label(root, text="Query 2", font=("Helvetica", 18))
        label_q2.pack()

        # # # Query 2

        q2_frame = tk.Frame(root, width = 1000, height = 1200)
        q2_frame.pack(expand=True, fill="both")

        q2_canvas_frame = tk.Frame(q2_frame)
        q2_canvas_frame.pack(side="left", fill="both", expand=True)

        self.query_text_canvas2 = tk.Canvas(q2_frame, bg = 'white')

        query_text_scrollbar_v2 = tk.Scrollbar(q2_frame, orient = tk.VERTICAL)
        query_text_scrollbar_v2.place(relx=0.49, rely=0.5, relwidth=0.01, relheight=0.5)
        query_text_scrollbar_v2.config(command=self.query_text_canvas2.yview)

        query_text_scrollbar_h2 = tk.Scrollbar(q2_frame, orient = tk.HORIZONTAL)
        query_text_scrollbar_h2.place(relx=0, rely=0.98, relwidth=0.5, relheight=0.02)
        query_text_scrollbar_h2.config(command=self.query_text_canvas2.xview)

        self.query_text_canvas2.config(yscrollcommand = query_text_scrollbar_v2.set, xscrollcommand = query_text_scrollbar_h2.set)
        self.query_text_canvas2.bind('<Configure>', lambda e: self.query_text_canvas2.configure(scrollregion=self.query_text_canvas2.bbox("all")))
        self.query_text_canvas2.place(relx=0, rely=0.5, relheight=0.48, relwidth=0.49)
        self.query_text_canvas2.create_text(250, 70, text='', tags='querytext', width=700)

        #query plan canvas
        self.canvas2 = tk.Canvas(q2_canvas_frame, bg='white', bd=2)

        scrollbar_v2 = tk.Scrollbar(q2_canvas_frame, orient = tk.VERTICAL, bg='white')
        scrollbar_v2.place(relx=0.99, rely=0, relheight=0.5, relwidth=0.01)
        scrollbar_v2.config(command=self.canvas2.yview)
        self.canvas2.config(yscrollcommand=scrollbar_v2.set)

        scrollbar_h2 = tk.Scrollbar(q2_canvas_frame, orient = tk.HORIZONTAL, bg='white')
        scrollbar_h2.place(relx=0, rely=0.48, relheight=0.02, relwidth=0.99)
        scrollbar_h2.config(command=self.canvas2.xview)
        self.canvas2.config(xscrollcommand=scrollbar_h2.set)

        self.canvas2.bind('<Configure>', lambda e: self.canvas2.configure(scrollregion=self.canvas2.bbox("all")))
        self.canvas2.place(relx=0, rely=0, relheight=0.48, relwidth=0.99)
        
        
        self.query_json_canvas2 = tk.Canvas(q2_frame, bg = 'white')
        query_json_scrollbar_v2 = tk.Scrollbar(q2_frame, orient = tk.VERTICAL)
        query_json_scrollbar_v2.place(relx=0.99, rely=0.5, relwidth=0.01, relheight=0.5)
        query_json_scrollbar_v2.config(command=self.query_json_canvas2.yview)

        query_json_scrollbar_h2 = tk.Scrollbar(q2_frame, orient = tk.HORIZONTAL)
        query_json_scrollbar_h2.place(relx=0.5, rely=0.98, relwidth=0.5, relheight=0.02)
        query_json_scrollbar_h2.config(command=self.query_json_canvas2.xview)

        self.query_json_canvas2.config(xscrollcommand=query_json_scrollbar_h2.set, yscrollcommand=query_json_scrollbar_v2.set)
        self.query_json_canvas2.bind('<Configure>', lambda e: self.query_json_canvas2.configure(scrollregion=self.query_json_canvas2.bbox("all")))
        self.query_json_canvas2.place(relx=0.5, rely=0.5, relheight=0.48, relwidth=0.49)
        self.query_json_canvas2.create_text(250, 70, text = '', tags="jsonquery", width=700)
        self.query_json_canvas2.bind_all("<MouseWheel>" , self.onScroll_json)
        



        
        # create file menu to exit 
        menu = tk.Menu(root)
        root.config(menu = menu)
        filemenu = tk.Menu(menu)
        menu.add_cascade(label = "File" , menu = filemenu)
        filemenu.add_command(label = "Exit" , command = close , accelerator="Ctrl+Q")
        filemenu.add_command(label="New Query", command=self.new_query)

        root.bind_all("<Control-q>" , lambda e : close())
        root.mainloop()
        
    def new_query(self):
        font = 'Consolas'

        row = [
    [
        sg.Text('Enter Query 1:', font=(font, 12), justification='left')
    ],
    [
        sg.Multiline(
            size=(45, 15),
            key="query1",
            font=(font, 12),
            autoscroll=True
        )
    ],
    [
        sg.Text('Enter Query 2:', font=(font, 12), justification='left')
    ],
    [
        sg.Multiline(
            size=(45, 15),
            key="query2",
            font=(font, 12),
            autoscroll=True
        )
    ],

]

        
        col1 = [[sg.Checkbox('Bitmap scan', key='bitmap_scan', default=True)],
                        [sg.Checkbox('Hashagg', key='hashagg', default=True)],
                        [sg.Checkbox('Hashjoin', key='hashjoin', default=True)],
                        [sg.Checkbox('Index scan', key='index_scan', default=True)]
                        ]
        
        col2 = [[sg.Checkbox('Index Only scan', key='index_only_scan', default=True)],
                        [sg.Checkbox('Material', key='material', default=True)],
                        [sg.Checkbox('Merge Join', key='merge_join', default=True)],
                        [sg.Checkbox('Nested Loop', key='nested_loop', default=True)]
        ]

        col3 = [[sg.Checkbox('Seq Scan', key='seq_scan', default=True)],
                        [sg.Checkbox('Sort', key='sort', default=True)],
                        [sg.Checkbox('Tidscan', key='tidscan', default=True)],
                        [sg.Checkbox('Gather merge', key='gather_merge', default=True)]
                        ]
        
        layout = [  
            row,
            [sg.Frame(layout=col1, title=''), sg.Frame(layout=col2, title=''), sg.Frame(layout=col3, title='')],
            [sg.Button('Submit')]
        ]

        window = sg.Window('CX4031 Project 2 GUI', layout, element_justification='c').Finalize()

        while True:
            event, values = window.read()
            if event == 'Submit' or event == "_Enter":   # when user clicks submit button
                
                if(values['query1'] != '') and (values['query2'] != ''):
                    self.query1 = values['query1'] #get Query1
                    self.query2 = values['query2'] # Get Query 2
                    self.query1 = self.structure_query(self.query1)
                    self.query2 = self.structure_query(self.query2)

                    self.json_query1= self.connect.generatePlan(self.query1, self.apqs1)
                    self.json_query2= self.connect.generatePlan(self.query2, self.apqs2)

                    if self.json_query1 != None and self.json_query2 != None:
                        self.apqs1 = [values['bitmap_scan'], values['hashagg'], values['hashjoin'], values['index_scan'], values['index_only_scan'], values['material'], values['merge_join'], values['nested_loop'], values['seq_scan'], values['sort'], values['tidscan'], values['gather_merge']]
                        self.apqs2 = [values['bitmap_scan'], values['hashagg'], values['hashjoin'], values['index_scan'], values['index_only_scan'], values['material'], values['merge_join'], values['nested_loop'], values['seq_scan'], values['sort'], values['tidscan'], values['gather_merge']]
                        break
                    else: 
                        tk.messagebox.showerror('Invalid Query', 'Invalid Query: Please Try Again.')
                else: 
                    tk.messagebox.showerror('Missing Query', 'Please enter 2 queries.')
            
                

            if event == sg.WIN_CLOSED: # if user closes window
                break


        if self.query1 != '' and self.query2 != '' and self.json_query1 != None and self.json_query2 != None:
            
            self.query1 = self.structure_query(self.query1)
            self.query2 = self.structure_query(self.query2)

            self.json_query1= self.connect.generatePlan(self.query1, self.apqs1)
            self.json_query2= self.connect.generatePlan(self.query2, self.apqs2)
            
            if self.json_query1 != None and self.json_query2 != None:
                clauseDict1 = self.getClause(self.query1)
                self.query_text_canvas.itemconfig("querytext", text = self.query1)
                self.canvas.delete('all')

                plan1 = ex.Plan(clauseDict1)
                plan1.draw(self.json_query1, self.canvas)
                self.query_json_canvas.itemconfig("jsonquery", text = " ".join(plan1.annotationList))

                
    
                self.query_json_canvas2.itemconfig("jsonquery", text = self.json_query2)

                clauseDict2 = self.getClause(self.query2)
                self.query_text_canvas2.itemconfig("querytext", text = self.query2)
                self.canvas2.delete('all')

                plan2 = ex.Plan(clauseDict2)
                plan2.draw(self.json_query2, self.canvas2)
                comparePlanDescription = ex.comparePlans(plan1,plan2)
                self.query_json_canvas2.itemconfig("jsonquery", text = " ".join(plan2.annotationList) + "\n" + comparePlanDescription)
                window.close()
           

        
        
    def getClause(self,query):
        diffClauses = ['where', 'and', 'having']

        numWhere = query.count('where')
        numAnd = query.count('and')
        numHaving = query.count('having')
        numLimit = query.count('limit')

        return {'where': numWhere, 'and': numAnd, 'having': numHaving, 'limit': numLimit}
            
        
        
        
        


    