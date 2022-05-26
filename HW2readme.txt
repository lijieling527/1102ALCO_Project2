建立兩個struct:
1.reg: 存放register name和value
2.predict: 存放predictor的state和3_bit_history以及miss的次數


建立四個vector<string>:
1.PC: 存放instruction的Program counter
2.type: 存放instruction的類型，ex:li, addi, beq......
3.inst: 存放instruction所用到的register、immedeate
4.label: 存放label名稱 

用剛剛的struct分別建立:
1.vector<reg> use_reg: 存放使用到的register和值
2.vector<predict> predictor: 有幾個entry就有幾個predictor

	

int outcome:0代表 Not taken，1代表Taken
int state: 當前的預測使用哪一個state
char pre: Taken = 'T'; Not taken = 'N'
string missprediction: 存放miss或not miss


1.讀檔並處理input.txt，再輸入entry，entry只能大於0，而且要2的次方數，如果不滿足就要再重新輸入

2.將剛剛處理好的input類型分別作處理
  (1)calc_li處理li
  (2)calc_addi處理addi
  (3)calc_beq處理beq

3.使用update()來更新predictor的狀態


