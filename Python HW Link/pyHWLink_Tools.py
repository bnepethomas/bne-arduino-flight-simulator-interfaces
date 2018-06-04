# pyHWLink_Tools.py

from datetime import datetime

def log_mS():
    return str(datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3] + ':')
