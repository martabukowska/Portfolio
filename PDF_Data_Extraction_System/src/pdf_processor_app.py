from flask import Flask, request, render_template, redirect, url_for, session, send_file
import pdfplumber
import pandas as pd
import io

app = Flask(__name__)
app.secret_key = 'your_secret_key'

USERNAME = 'FHLLP'
PASSWORD = 'FH40WallSt!'

@app.route('/', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        username = request.form['username']
        password = request.form['password']
        if username == USERNAME and password == PASSWORD:
            session['logged_in'] = True
            return redirect(url_for('upload_file'))
        else:
            return "Invalid credentials, please try again."
    return render_template('login.html')

@app.route('/upload', methods=['GET', 'POST'])
def upload_file():
    if not session.get('logged_in'):
        return redirect(url_for('login'))

    if request.method == 'POST':
        if 'file' not in request.files:
            return 'No file part'
        file = request.files['file']
        if file.filename == '':
            return 'No selected file'
        if file:
            pdf_bytes = file.read()

            tables, page_numbers = extract_tables_from_pdf(pdf_bytes)
            if tables:
                clean_tables = clean_data(tables, page_numbers)
                final_clean_tables = handle_duplicate_rows(clean_tables)

                # Deleting unnecessary variables
                del tables
                del page_numbers
                del clean_tables

                csv_bytes = save_tables_to_csv(final_clean_tables)

                # Deleting the final cleaned tables
                del final_clean_tables

                return send_file(
                    io.BytesIO(csv_bytes),
                    mimetype='text/csv',
                    as_attachment=True,
                    download_name='output.csv'
                )
            else:
                return 'No tables found in the PDF.'

    return render_template('upload.html')

@app.route('/logout')
def logout():
    session.pop('logged_in', None)
    return redirect(url_for('login'))

def extract_tables_from_pdf(pdf_bytes):
    tables = []
    page_numbers = []
    with pdfplumber.open(io.BytesIO(pdf_bytes)) as pdf:
        for page_number, page in enumerate(pdf.pages, start=1):
            table = page.extract_table()
            if table:
                if not tables:
                    header = ["Identifier"] + table[0]
                    tables.append(header)
                    page_numbers.append(page_number)

                for row in table[1:]:
                    identifier = f"Page {page_number}"
                    tables.append([identifier] + row)
                    page_numbers.append(page_number)

    return tables, page_numbers

def is_correct_row(row):
    return len(row) >= 4 and ' ' in row[-1] and ' ' in row[-2]

def should_skip_merging(row):
    return "Unassigned" in row[-1:] or "Unassigned" in row[-2:]

def clean_data(tables, page_numbers):
    clean_tables = []
    num_columns = len(tables[0])
    clean_tables.append(tables[0])

    previous_row = None
    previous_page_number = None

    i = 1
    while i < len(tables):
        current_row = [str(item).replace('\n', ' ').replace('\r', '') for item in tables[i]]
        current_page_number = page_numbers[i]

        if previous_row is not None and current_page_number != previous_page_number:
            if not should_skip_merging(previous_row) and not should_skip_merging(current_row):
                if not is_correct_row(previous_row) or not is_correct_row(current_row):
                    for j in range(1, num_columns):
                        previous_row[j] = (previous_row[j] + ' ' + current_row[j]).strip()
                    current_row = previous_row

        clean_tables.append(current_row)

        previous_row = current_row
        previous_page_number = current_page_number
        i += 1

    return clean_tables

def handle_duplicate_rows(clean_tables):
    unique_clean_tables = []
    seen_rows = set()

    for row in clean_tables:
        row_tuple = tuple(row[1:])
        if row_tuple not in seen_rows:
            unique_clean_tables.append(row)
            seen_rows.add(row_tuple)

    return unique_clean_tables

def save_tables_to_csv(tables):
    tables_without_identifier = [row[1:] for row in tables]
    df = pd.DataFrame(tables_without_identifier[1:], columns=tables_without_identifier[0])
    csv_buffer = io.StringIO()
    df.to_csv(csv_buffer, index=False)
    return csv_buffer.getvalue().encode()

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=8080)
