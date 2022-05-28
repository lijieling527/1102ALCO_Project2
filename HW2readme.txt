建立兩個struct:
1.reg: 存放register name和value
2.predict: 存放predictor的目前的state、預測的結果、3_bit_history的值以及miss prediction的次數


建立四個vector<string>:
1.PC: 存放instruction的Program counter，ex: 0x000
2.type: 存放instruction的類型，ex:li, addi, beq......
3.inst: 存放instruction所用到的register，ex:R3,R4
4.label: 存放label名稱，ex:LOOP,END

用剛剛的struct分別建立:
1.vector<reg> use_reg: 存放使用到的register和其值
2.vector<predict> predictor: 輸入幾個entry就有幾個predictor

	
int outcome:0代表 Not taken，1代表Taken



1.讀檔並呼叫finput()來處理input.txt，將input的內容分別放入剛剛建立好的PC、type、inst、label的vector裡，將PC裡的值轉換成十進位置，再輸入entry，entry只能大於0，而且要2的次方數，如果不滿足就要再重新輸入

2.將剛剛處理好的PC、type、inst、label分別作處理，呼叫ALU()，裡面會分別執行
  (1)li()處理li
  (2)addi()處理addi
  (3)beq()處理beq

3.使用prediction()來更新predictor的狀態
