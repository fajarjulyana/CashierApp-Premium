#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/choice.h>
#include <sqlite3.h>
#include <xlsxwriter.h>  // Include the xlsxwriter library
#include <fstream>
#include <vector>
#include <cstdlib>
#include <wx/msgdlg.h>

class MyFrame : public wxFrame {
public:
    MyFrame(const wxString& title);

private:
    wxGrid* m_grid;
    wxChoice* m_dayChoice;
    wxChoice* m_monthChoice;
    wxChoice* m_yearChoice;
    sqlite3* db;
    sqlite3_stmt* stmt;
    wxStaticText* m_totalIncomeText;
    wxStaticText* m_totalProfitText;
    double totalIncome = 0.0;
    double totalProfit = 0.0;

    void PopulateDateChoices();
    void FetchDataAndUpdateGrid();
    void GenerateExcelReport(const std::string& filename);
    void OnPrint(wxCommandEvent& event);
    void OnFilterChanged(wxCommandEvent& event);
};

MyFrame::MyFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {
    wxPanel* panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    // Filter dropdowns
    wxBoxSizer* filterSizer = new wxBoxSizer(wxHORIZONTAL);
    m_dayChoice = new wxChoice(panel, wxID_ANY);
    m_monthChoice = new wxChoice(panel, wxID_ANY);
    m_yearChoice = new wxChoice(panel, wxID_ANY);

    filterSizer->Add(new wxStaticText(panel, wxID_ANY, "Hari:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    filterSizer->Add(m_dayChoice, 1, wxRIGHT, 10);
    filterSizer->Add(new wxStaticText(panel, wxID_ANY, "Bulan:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    filterSizer->Add(m_monthChoice, 1, wxRIGHT, 10);
    filterSizer->Add(new wxStaticText(panel, wxID_ANY, "Tahun:"), 0, wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
    filterSizer->Add(m_yearChoice, 1);

    vbox->Add(filterSizer, 0, wxEXPAND | wxALL, 10);

    // Grid
    m_grid = new wxGrid(panel, wxID_ANY);
    m_grid->CreateGrid(0, 4);
    m_grid->SetColLabelValue(0, "Nama Barang");
    m_grid->SetColLabelValue(1, "Jumlah");
    m_grid->SetColLabelValue(2, "Pendapatan");
    m_grid->SetColLabelValue(3, "Keuntungan");
    vbox->Add(m_grid, 1, wxEXPAND | wxALL, 10);

    // Total income and profit
    m_totalIncomeText = new wxStaticText(panel, wxID_ANY, "Total Pendapatan: 0.00");
    m_totalProfitText = new wxStaticText(panel, wxID_ANY, "Total Keuntungan: 0.00");
    vbox->Add(m_totalIncomeText, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxTOP, 10);
    vbox->Add(m_totalProfitText, 0, wxALIGN_LEFT | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    // Print button
    wxButton* printButton = new wxButton(panel, wxID_ANY, "Cetak Laporan");
    vbox->Add(printButton, 0, wxALIGN_RIGHT | wxALL, 10);

    panel->SetSizer(vbox);

    // Event bindings
    printButton->Bind(wxEVT_BUTTON, &MyFrame::OnPrint, this);
    m_dayChoice->Bind(wxEVT_CHOICE, &MyFrame::OnFilterChanged, this);
    m_monthChoice->Bind(wxEVT_CHOICE, &MyFrame::OnFilterChanged, this);
    m_yearChoice->Bind(wxEVT_CHOICE, &MyFrame::OnFilterChanged, this);

    // Populate dropdowns and fetch initial data
    PopulateDateChoices();
    FetchDataAndUpdateGrid();
}

void MyFrame::PopulateDateChoices() {
    m_dayChoice->Append("Semua");
    m_monthChoice->Append("Semua");
    m_yearChoice->Append("Semua");

    for (int day = 1; day <= 31; ++day) {
        m_dayChoice->Append(wxString::Format("%02d", day));
    }

    for (int month = 1; month <= 12; ++month) {
        m_monthChoice->Append(wxString::Format("%02d", month));
    }

    // Generate years dynamically
    if (sqlite3_open("kasir.db", &db) == SQLITE_OK) {
        const char* yearQuery = "SELECT DISTINCT strftime('%Y', tanggal) AS year FROM transaksi_log ORDER BY year ASC";
        if (sqlite3_prepare_v2(db, yearQuery, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                m_yearChoice->Append(wxString::FromUTF8((const char*)sqlite3_column_text(stmt, 0)));
            }
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);
    }
}

void MyFrame::FetchDataAndUpdateGrid() {
    if (m_grid->GetNumberRows() > 0) {
        m_grid->DeleteRows(0, m_grid->GetNumberRows());
    }

    totalIncome = 0.0;
    totalProfit = 0.0;

    if (sqlite3_open("kasir.db", &db)) {
        wxMessageBox("Cannot open database: " + wxString(sqlite3_errmsg(db)), "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Construct date filter
    wxString dateFilter = "";
    if (m_yearChoice->GetSelection() > 0) {
        dateFilter += m_yearChoice->GetStringSelection();
        if (m_monthChoice->GetSelection() > 0) {
            dateFilter += "-";
            dateFilter += m_monthChoice->GetStringSelection();
            if (m_dayChoice->GetSelection() > 0) {
                dateFilter += "-";
                dateFilter += m_dayChoice->GetStringSelection();
            }
        }
    } else {
        dateFilter = "%"; // Default to all dates
    }

    std::string query = "SELECT dt.jumlah, dt.harga, b.harga_beli, b.nama FROM detail_transaksi dt "
                        "JOIN barang b ON dt.id_barang = b.id "
                        "JOIN transaksi_log t ON t.id = dt.transaksi_id "
                        "WHERE t.tanggal LIKE '" + std::string(dateFilter.mb_str()) + "%'";

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, 0) != SQLITE_OK) {
        wxMessageBox("Failed to fetch data: " + wxString(sqlite3_errmsg(db)), "Error", wxOK | wxICON_ERROR);
        sqlite3_close(db);
        return;
    }

    int row = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int jumlah = sqlite3_column_int(stmt, 0);
        double hargaJual = sqlite3_column_double(stmt, 1);
        double hargaBeli = sqlite3_column_double(stmt, 2);
        const char* nama = (const char*)sqlite3_column_text(stmt, 3);

        double incomePerItem = hargaJual * jumlah;
        double profitPerItem = (hargaJual - hargaBeli) * jumlah;

        totalIncome += incomePerItem;
        totalProfit += profitPerItem;

        m_grid->AppendRows(1);
        m_grid->SetCellValue(row, 0, wxString(nama));
        m_grid->SetCellValue(row, 1, wxString::Format("%d", jumlah));
        m_grid->SetCellValue(row, 2, wxString::Format("%.2f", incomePerItem));
        m_grid->SetCellValue(row, 3, wxString::Format("%.2f", profitPerItem));

        row++;
    }

    m_totalIncomeText->SetLabel(wxString::Format("Total Pendapatan: %.2f", totalIncome));
    m_totalProfitText->SetLabel(wxString::Format("Total Keuntungan: %.2f", totalProfit));

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void MyFrame::GenerateExcelReport(const std::string& filename) {
    lxw_workbook  *workbook  = workbook_new(filename.c_str());
    lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);

    // Set header style
    lxw_format *header_format = workbook_add_format(workbook);
    format_set_bold(header_format);
    format_set_align(header_format, LXW_ALIGN_CENTER);
    format_set_align(header_format, LXW_ALIGN_VERTICAL_CENTER);
    format_set_bg_color(header_format, LXW_COLOR_GRAY);
    format_set_border(header_format, LXW_BORDER_THIN);
    format_set_font_size(header_format, 12);

    // Title format
    lxw_format *title_format = workbook_add_format(workbook);
    format_set_bold(title_format);
    format_set_font_size(title_format, 16);  // Larger font for the title
    format_set_align(title_format, LXW_ALIGN_CENTER);
    format_set_align(title_format, LXW_ALIGN_VERTICAL_CENTER);

    // Write title
    worksheet_write_string(worksheet, 0, 0, "Laporan Keuangan", title_format);
    worksheet_merge_range(worksheet, 0, 0, 0, 3, "Laporan Keuangan", title_format);

    // Write headers
    worksheet_write_string(worksheet, 1, 0, "Nama Barang", header_format);
    worksheet_write_string(worksheet, 1, 1, "Jumlah", header_format);
    worksheet_write_string(worksheet, 1, 2, "Pendapatan", header_format);
    worksheet_write_string(worksheet, 1, 3, "Keuntungan", header_format);

    // Set column widths based on the length of the header titles
    worksheet_set_column(worksheet, 0, 0, 12, NULL); // "Nama Barang" width
    worksheet_set_column(worksheet, 1, 1, 10, NULL); // "Jumlah" width
    worksheet_set_column(worksheet, 2, 2, 12, NULL); // "Pendapatan" width
    worksheet_set_column(worksheet, 3, 3, 12, NULL); // "Keuntungan" width

    // Write data
    int row = 2;
    double totalIncome = 0.0;
    double totalProfit = 0.0;

    for (int i = 0; i < m_grid->GetNumberRows(); ++i) {
        lxw_format *row_format = workbook_add_format(workbook);

        // Alternate row colors
        if (row % 2 == 0) {
            format_set_bg_color(row_format, LXW_COLOR_WHITE);
        } else {
            format_set_bg_color(row_format, LXW_COLOR_GRAY);
        }
        format_set_border(row_format, LXW_BORDER_THIN);

        // Write row data
        std::string nama_barang = m_grid->GetCellValue(i, 0).ToStdString();
        int jumlah = std::stoi(m_grid->GetCellValue(i, 1).ToStdString());
        double pendapatan = std::stod(m_grid->GetCellValue(i, 2).ToStdString());
        double keuntungan = std::stod(m_grid->GetCellValue(i, 3).ToStdString());

        worksheet_write_string(worksheet, row, 0, nama_barang.c_str(), row_format);
        worksheet_write_number(worksheet, row, 1, jumlah, row_format);
        worksheet_write_number(worksheet, row, 2, pendapatan, row_format);
        worksheet_write_number(worksheet, row, 3, keuntungan, row_format);

        totalIncome += pendapatan;
        totalProfit += keuntungan;

        row++;
    }

    // Add totals below the data table
    lxw_format *footer_format = workbook_add_format(workbook);
    format_set_bold(footer_format);
    format_set_align(footer_format, LXW_ALIGN_CENTER);
    format_set_bg_color(footer_format, LXW_COLOR_YELLOW);  // Highlight the total row
    format_set_border(footer_format, LXW_BORDER_THIN);

    // Merge cells and write totals
    worksheet_merge_range(worksheet, row, 0, row, 2, "Total Pendapatan", footer_format);
    worksheet_write_number(worksheet, row, 3, totalIncome, footer_format);

    worksheet_merge_range(worksheet, row + 1, 0, row + 1, 2, "Total Keuntungan", footer_format);
    worksheet_write_number(worksheet, row + 1, 3, totalProfit, footer_format);

    // Save the workbook
    workbook_close(workbook);
}

void MyFrame::OnPrint(wxCommandEvent& event) {
    // Nama file Excel dan PDF
    const std::string excel_file = "laporan.xlsx";
    const std::string pdf_file = "laporan.pdf";

    // Generate laporan Excel
    GenerateExcelReport(excel_file);

    // Konversi file Excel ke PDF menggunakan LibreOffice
    std::string convert_command = "libreoffice --headless --convert-to pdf " + excel_file;
    int convert_result = system(convert_command.c_str());
    if (convert_result != 0) {
        wxMessageBox("Gagal mengonversi file Excel ke PDF.", "Error", wxICON_ERROR);
        return;
    }

    // Cetak file PDF menggunakan lp (CUPS)
    std::string print_command = "lp " + pdf_file;
    int print_result = system(print_command.c_str());
    if (print_result != 0) {
        wxMessageBox("Gagal mencetak file PDF.", "Error", wxICON_ERROR);
        return;
    }

    wxMessageBox("Laporan berhasil dicetak.", "Informasi", wxICON_INFORMATION);
}

void MyFrame::OnFilterChanged(wxCommandEvent& event) {
    // Mengambil nilai yang dipilih dari dropdown filter
    int selectedDay = m_dayChoice->GetSelection();
    int selectedMonth = m_monthChoice->GetSelection();
    int selectedYear = m_yearChoice->GetSelection();

    // Memperbarui grid dan menampilkan data berdasarkan filter yang dipilih
    FetchDataAndUpdateGrid();
}

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        MyFrame* frame = new MyFrame("Laporan Keuangan");
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

