# PDF Data Extraction System

Web-based data recovery application using Python (Flask), processing and reconstructing fragmented PDF records during critical system outage.

## Features
- Table extraction system using pdfplumber and pandas
- Content-based merging algorithm for split records
- Automated CSV export functionality
- Secure authentication system
- Cross-page data reconstruction

## Technologies
- Python 3.x
- Flask 2.1.2
- Gunicorn 20.1.0
- pdfplumber 0.5.28
- pandas 1.3.3
- NumPy 1.21.4
- Werkzeug 2.0.3

## Setup
1. Install requirements:
   ```bash
   pip install -r requirements.txt
   ```

2. Create config.py with demo credentials:
   ```python
   USERNAME = 'demo_user'
   PASSWORD = 'demo123'
   ```

3. Run the application:
   ```bash
   python src/pdf_processor_app.py
   ```

Note: For production use, create a separate `config_prod.py` with secure credentials (this file is gitignored).

## Live Demo

A production version of this application is deployed at: https://tocsv.onrender.com/

Note: While the login interface is publicly accessible, authentication is restricted to authorized users only. For security reasons, public access is limited to viewing the login interface.

> **Please note**: The application is hosted on a free-tier server, so initial loading may take 30-60 seconds when the server spins up from idle.

For development and testing purposes:
- Demo credentials are available in the config.py file
- Local setup instructions are provided above

## Author
Marta Bukowska