### 0. user id 얻어내기
> 2021.11.24 구현 완료: 00_user_id directory

<br>
<br>

### 1. message parsing 구현
> 2021.11.24 구현 완료: 01_message_parser directory

#### A. 0x10 타입에 필요한 정보들이 주어졌을 때, sending_message 만들기
#### B. 0x11 타입의 예시가 주어졌을 때, 필요한 정보들을 분리하여 반환하기
#### C. data의 size 계산하기

<br>
<br>

### 2. option select & 분기를 구현
> 2021.11.26 구현 완료: 02_option
<br>
<br>

### 3. pthread 구현
> 2021.11.29 구현 완료: 03_pthread
<br>
<br>

### 3. file read/write test
> 2021.11.30 구현 완료: 04_file
<br>
<br>

### 4. msg_type 별 분기가 작동하도록 구현한다
#### A. send message
> 2021.11.26 구현 완료: 02_option
    0x10: send message
<br>
  
#### B. recv message
> 2021.11.29 구현 완료: 03_pthread
    0x11: recv message
<br>
  
#### C. file uploading
    0x20: send request (uploading file)
    0x21: recv responce (msg_end: 0x00 = ok / 0x01 = no)
    0x22: send file data (msg_end: 0x00 = last one / 0x01 = sending)
    0x23: recv responce
<br>

#### D. file downloading
    0x30: send request (downloading file)
    0x31: recv responce (msg_end: 0x00 = ok / 0x01 = no)
    0x32: recv file data (msg_end: 0x00 = last one / 0x01 = sending)
    0x33: send responce
<br>

#### E. request uploaded file list
    0x40: send request (file list)
    0x41: recv responce (msg_end: 0x00 = last one / 0x01 = sending)
<br>

#### F. close connection
    0x50: send connection closing message (data: EXIT)
    0x51: recv responce (msg_end: 0x00 = ok)
<br>
<br>

### 5. 각 msg_type 별 테스트를 진행한다
