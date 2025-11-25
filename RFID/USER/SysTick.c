#include "SysTick.h"


volatile static uint32_t ms; // kiểu dữ liệu để mà báo cho cái trình biên dịch là cái biến này có thể thay đổi ở bát kò đaia 


/*
	- SystemCoreClock  = 72MHz
	- có thể thay đổi giá trị của bộ chia bằng các tham số theo ý muốn
	- nếu muốn xảy ra sự kiện ngắt sau 1ms thì SysTick_Config(SystemCoreClock / 1000) sẽ tạo ra ngắt sau mỗi 1ms
		với công thức tính này thì 
		72MHz / 1000 = 72000 -> 0
		và giá trị 72000 này sẽ được nạp vào thanh ghi LOAD của SysTick có nghĩa là SysTick sẽ đếm từ 72000 về 0
 		khi đếm về 0 thì sẽ tạo ra ngắt SysTick_Handler()
 	-> do đó SysTick_Config(SystemCoreClock / 1000) sẽ tạo ra ngắt sau mỗi 1ms

	tương tự nếu muốn tạo ra ngắt sau mỗi 10ms thì ta sẽ sử dụng SysTick_Config(SystemCoreClock / 100)
	tạo ra ngắt 1us thì ta sẽ sử dụng SysTick_Config(SystemCoreClock / 1000000)

	nếu muốn tạo ra ngắt chu kì 2us thì ta sẽ sử dụng SysTick_Config(SystemCoreClock / 500000)
*/
void systick_init(void)
{
	SysTick_Config(SystemCoreClock / 1000); // khi dem tu 72000 -> 0 dem ve 0 thi bat co len 
}

/*
	khi xảy ra sự kiện ngắt thì sẽ tăng biến ms lên 1
 - biến us này sẽ được sử dụng để tính thời gian trôi qua theo đơn vị ms
*/
void SysTick_Handler()
{
	++ms; 
}
	
/*
	hàm này sẽ trả về giá trị của biến ms tương tự với hàm millis() trong Arduino
*/
uint32_t millis(void) // arduino -> hàm này ở arduino hoặc esp nó trả về giá trị mà băngf số tick mà nó chạy đc từ khi mà bắt đầu cấp nguồn vào con mcu 
{
	//return ((ms / 1000) * 2); // phải chia 1000 và nhân với 2 vì SysTick_Config(SystemCoreClock / 500000) tạo ra ngắt mỗi 2us
	return ms;
}

/*
	để tạo ra độ trễ thì ta sẽ sử dụng hàm delay_ms() với tham số là thời gian cần trễ tính bằng ms
 - hàm này sẽ sử dụng hàm millis() để lấy thời gian hiện tại và so sánh với thời gian hiện tại + thời gian trễ
 - khi thời gian hiện tại + thời gian trễ lớn hơn thời gian hiện tại thì sẽ
   tiếp tục vòng lặp cho đến khi thời gian hiện tại + thời gian trễ nhỏ hơn thời gian hiện tại
*/
void delay_ms(uint32_t time)
{
	uint32_t current_time = millis();
	while(millis() - current_time < time); 
}