# TÀI LIỆU THIẾT KẾ HỆ THỐNG QUẢN LÝ KHO RFID (FINAL)
**Phần cứng:** RC522 - STM32 - ESP32

---

## 1. Tổng quan Hệ thống
Hệ thống quản lý kho thông minh sử dụng công nghệ RFID tần số cao (HF 13.56 MHz).
- **STM32 (Reader Node):** Đọc thẻ RFID, xác định loại thẻ (Sản phẩm hay Vị trí), đóng gói bản tin chuẩn và gửi qua UART. Điều khiển đèn/còi báo trạng thái.
- **ESP32 (Gateway & Logic):** Nhận dữ liệu từ STM32, thực hiện logic kiểm tra UID với Cloud, ghép nối dữ liệu (Pairing) và đồng bộ qua MQTT.
- **Giao tiếp:** UART (Baudrate 115200).

---

## 2. Chiến lược Dữ liệu (Data Strategy)
Hệ thống kết hợp dữ liệu cứng trên thẻ và dữ liệu mềm trên Cloud.

### 2.1. Dữ liệu trên thẻ RFID (Block Memory)
* **Thẻ Sản phẩm (Tag Infor):** Lưu `Mã sản phẩm` (Ví dụ: 0x01, 0x02...) và `Trạng thái`.
* **Thẻ Vị trí (Tag Location):** Lưu tọa độ kho gồm `Khu`, `Dãy`, `Kệ`.

### 2.2. Dữ liệu trên Cloud
Lưu trạng thái thực tế của từng UID:
* `UID: A1-B2-C3-D4` -> `Mã 0x01` (Tivi Sony) -> **Status: Active**.
* `UID: XX-YY-ZZ-00` -> **Status: Unregistered** (Chưa đăng ký).

---

## 3. Quy trình Nghiệp vụ & Kịch bản Chạy (Execution Flow)

### 3.1. Kịch bản NHẬP KHO (Inbound)
*Trạng thái: ESP32 gửi lệnh `CMD: IN` -> STM32 đèn Vàng.*

#### **Bước 1: Quét thẻ Sản phẩm (User thao tác)**
STM32 đọc thẻ, gửi bản tin **DATA: Infor** lên ESP32.

* **Tình huống A: UID Chưa đăng ký (Thẻ lạ)**
    * **Cloud:** Trả về `Error: Unregistered`.
    * **ESP32:** Gửi lệnh `CMD: ERROR` xuống STM32. Xóa bộ nhớ đệm.
    * **STM32:** **Còi báo động (Tít-tít-tít) + Đèn đỏ.** (Yêu cầu đổi thẻ).

* **Tình huống B: UID Hợp lệ (Thẻ quen)**
    * **Cloud:** Trả về `OK`.
    * **ESP32:** Lưu tạm `UID` và `Mã SP` vào bộ nhớ đệm (Cache).
    * **STM32:** (Tiếp tục đèn Vàng chờ đợi).

#### **Bước 2: Quét thẻ Vị trí (Chỉ thực hiện khi Bước 1 OK)**
STM32 đọc thẻ, gửi bản tin **DATA: Location** lên ESP32.

* **Xử lý:** ESP32 lấy `Mã SP` (từ bộ nhớ đệm) ghép với `Vị trí` vừa nhận -> Gửi API cập nhật lên Cloud.
* **Kết quả:**
    * Cloud xác nhận thành công.
    * ESP32 gửi lệnh `CMD: DONE` xuống STM32.
    * **STM32:** **Đèn Xanh sáng + Bíp dài.** (Hoàn tất nhập kho).

* **Ngoại lệ (Timeout):** Nếu sau 15s kể từ Bước 1 mà không quét thẻ Vị trí -> ESP32 tự xóa bộ nhớ đệm và gửi `CMD: ERROR` để reset quy trình.

---

### 3.2. Kịch bản XUẤT KHO (Outbound)
*Trạng thái: ESP32 gửi lệnh `CMD: OUT` -> STM32 đèn Vàng.*
*Giả định: Đơn hàng cần xuất `0x02` (Tủ lạnh).*

#### **Thao tác: Quét thẻ Sản phẩm**
STM32 gửi bản tin **DATA: Infor** lên ESP32.

* **Tình huống A: Thẻ lạ (Unregistered)**
    * **Xử lý:** Gửi `CMD: ERROR` -> **Còi báo động.**

* **Tình huống B: Sai hàng (Wrong Item)**
    * *Ví dụ:* Quét trúng Tivi (`0x01`).
    * **ESP32:** So sánh `0x01` != `0x02`.
    * **Xử lý:** Gửi `CMD: ERROR` -> **Còi báo động.**

* **Tình huống C: Đúng hàng (Correct Item)**
    * *Ví dụ:* Quét trúng Tủ lạnh (`0x02`).
    * **ESP32:** So sánh `0x02` == `0x02`. Cập nhật Cloud.
    * **Xử lý:** Gửi `CMD: DONE` -> **Đèn Xanh sáng.**

---

## 4. Cấu trúc Bản tin Truyền nhận (Protocol)
*Giữ nguyên cấu trúc byte như tài liệu gốc.*

### 4.1. Bản tin Điều khiển (CMD Message)
**Hướng:** ESP32 -> STM32 (8 Byte cố định)

| start (2 byte) | type (1 byte) | action (1 byte) | length (2 byte) | checksum (2 byte) |
| :--- | :--- | :--- | :--- | :--- |
| **0xAA55** | **0x00** | **[Action]** | **0x0008** | **[CRC]** |

* **Action Code:**
    * `0x00`: Mode IN (Nhập kho).
    * `0x01`: Mode OUT (Xuất kho).
    * `0x03`: Báo Lỗi/Check (Kêu còi).
    * `0xFF`: Hoàn thành/Done (Đèn xanh).
* **Length:** Cố định là 8 (0x0008).
* **Checksum:** Tổng kiểm tra (trừ 2 byte cuối).

**Ví dụ (Báo lỗi):** `AA 55 00 03 00 08 [CRC]`

---

### 4.2. Bản tin Dữ liệu (DATA Message)
**Hướng:** STM32 -> ESP32
Cấu trúc chung:
`| start | type | tag | length | DATA... | checksum |`

#### **A. Loại Infor (Thẻ Sản phẩm) - Tổng 13 Byte**
| start | type | tag | length | UID (4 byte) | Mã SP | Status | checksum |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **AA 55** | **01** | **00** | **00 0D** | **XX XX XX XX** | **01** | **00** | **[CRC]** |

* **Tag:** `0x00`
* **Length:** `0x000D` (13 byte)
* **Data:** UID (4 byte) + Mã SP (1 byte) + Status (1 byte).

#### **B. Loại Location (Thẻ Vị trí) - Tổng 11 Byte**
| start | type | tag | length | Khu | Dãy | Kệ | checksum |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **AA 55** | **01** | **01** | **00 0B** | **0A** | **01** | **02** | **[CRC]** |

* **Tag:** `0x01`
* **Length:** `0x000B` (11 byte)
* **Data:** Khu (1 byte) + Dãy (1 byte) + Kệ (1 byte).

---

## 5. Bảng Mã Quy Ước (Lookup Tables)

### 5.1. Mã Action (Trong CMD)
| Tên Define | Hex | Ý nghĩa |
| :--- | :--- | :--- |
| **CMD_IN** | `0x00` | Chuyển sang chế độ Nhập kho. |
| **CMD_OUT** | `0x01` | Chuyển sang chế độ Xuất kho. |
| **CMD_ERROR** | `0x03` | Báo lỗi (Còi kêu, Đèn đỏ). |
| **CMD_DONE** | `0xFF` | Hoàn thành (Đèn xanh, Reset trạng thái). |

### 5.2. Mã Tag (Trong DATA)
| Tên Define | Hex | Ý nghĩa |
| :--- | :--- | :--- |
| **TAG_INFOR** | `0x00` | Gói tin chứa thông tin sản phẩm. |
| **TAG_LOC** | `0x01` | Gói tin chứa thông tin vị trí. |

### 5.3. Mã Sản phẩm & Trạng thái (Ví dụ)
| Tên Define | Hex | Ghi chú |
| :--- | :--- | :--- |
| **SP_TV_SONY** | `0x01` | Sản phẩm mẫu 1. |
| **SP_TULANH** | `0x02` | Sản phẩm mẫu 2. |
| **STATUS_OK** | `0x00` | Trạng thái thẻ hợp lệ. |
| **STATUS_ERR** | `0x01` | Trạng thái lỗi/khóa. |

---

## 6. Checklist Kiểm thử (Test Cases)

| ID | Tên Kịch bản | Mô tả tóm tắt | Kết quả mong đợi |
| :--- | :--- | :--- | :--- |
| **TC-01** | **Nhập kho chuẩn** | ESP32 gửi `CMD_IN`. Quét SP đúng -> Quét Vị trí đúng. | ESP32 gửi `CMD_DONE`. Đèn xanh sáng. Cloud cập nhật vị trí. |
| **TC-02** | **Thẻ lạ (Unknown)** | ESP32 gửi `CMD_IN/OUT`. Quét thẻ chưa đăng ký UID trên Cloud. | ESP32 gửi `CMD_ERROR`. Còi báo động. Không lưu cache. |
| **TC-03** | **Xuất sai hàng** | ESP32 gửi `CMD_OUT` (yêu cầu A). Quét thẻ B (dù thẻ B xịn). | ESP32 gửi `CMD_ERROR`. Còi báo động. Màn hình báo "Wrong Item". |
| **TC-04** | **Quên quy trình** | Đang Nhập kho, quét SP xong bỏ đi (Timeout 15s). | ESP32 tự xóa Cache. Gửi `CMD_ERROR` reset trạng thái. |