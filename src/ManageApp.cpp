#include <wx/wx.h>
#include <wx/grid.h>
#include <sqlite3.h>

class BarangFrame : public wxFrame {
public:
    BarangFrame(const wxString& title);
    virtual ~BarangFrame();

private:
    wxTextCtrl* txtID;
    wxTextCtrl* txtNama;
    wxTextCtrl* txtHarga;
    wxTextCtrl* txtHargaBeli;
    wxTextCtrl* txtJumlah;
    wxGrid* gridBarang;
    wxTextCtrl* txtSearch;

    sqlite3* db;
    const char* dbName = "kasir.db";

    void OnSimpanBarang(wxCommandEvent& event);
    void OnEditBarang(wxCommandEvent& event);
    void OnHapusBarang(wxCommandEvent& event);
    void OnGridClick(wxGridEvent& event);
    void OnSearch(wxCommandEvent& event);
    void SetupDatabase();
    void LoadDataToGrid(const wxString& searchQuery = "");
    bool SimpanBarang(const wxString& id, const wxString& nama, double harga, double hargaBeli, int jumlah);
    bool EditBarang(const wxString& id, const wxString& nama, double harga, double hargaBeli, int jumlah);
    bool HapusBarang(const wxString& id);

    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(BarangFrame, wxFrame)
    EVT_BUTTON(1001, BarangFrame::OnSimpanBarang)
    EVT_BUTTON(1002, BarangFrame::OnEditBarang)
    EVT_BUTTON(1003, BarangFrame::OnHapusBarang)
    EVT_GRID_CELL_LEFT_CLICK(BarangFrame::OnGridClick)
    EVT_TEXT_ENTER(2001, BarangFrame::OnSearch)
wxEND_EVENT_TABLE()

BarangFrame::BarangFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) {

    sqlite3_open(dbName, &db);
    SetupDatabase();

    wxPanel* panel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Search bar
    wxBoxSizer* searchSizer = new wxBoxSizer(wxHORIZONTAL);
    txtSearch = new wxTextCtrl(panel, 2001, "", wxDefaultPosition, wxSize(300, 30), wxTE_PROCESS_ENTER);
    searchSizer->Add(new wxStaticText(panel, wxID_ANY, "Cari Barang: "), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    searchSizer->Add(txtSearch, 1, wxALL, 5);
    mainSizer->Add(searchSizer, 0, wxEXPAND);

    // Form inputs
    wxBoxSizer* formSizer = new wxBoxSizer(wxHORIZONTAL);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "ID:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    txtID = new wxTextCtrl(panel, wxID_ANY);
    formSizer->Add(txtID, 1, wxALL, 5);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Nama:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    txtNama = new wxTextCtrl(panel, wxID_ANY);
    formSizer->Add(txtNama, 1, wxALL, 5);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Harga:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    txtHarga = new wxTextCtrl(panel, wxID_ANY);
    formSizer->Add(txtHarga, 1, wxALL, 5);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Harga Beli:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    txtHargaBeli = new wxTextCtrl(panel, wxID_ANY);
    formSizer->Add(txtHargaBeli, 1, wxALL, 5);

    formSizer->Add(new wxStaticText(panel, wxID_ANY, "Jumlah:"), 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
    txtJumlah = new wxTextCtrl(panel, wxID_ANY);
    formSizer->Add(txtJumlah, 1, wxALL, 5);

    mainSizer->Add(formSizer, 0, wxEXPAND);

    // Buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    wxButton* btnSimpan = new wxButton(panel, 1001, "Simpan");
    buttonSizer->Add(btnSimpan, 1, wxALL, 5);

    wxButton* btnEdit = new wxButton(panel, 1002, "Edit");
    buttonSizer->Add(btnEdit, 1, wxALL, 5);

    wxButton* btnHapus = new wxButton(panel, 1003, "Hapus");
    buttonSizer->Add(btnHapus, 1, wxALL, 5);

    mainSizer->Add(buttonSizer, 0, wxEXPAND);

    // Grid
    gridBarang = new wxGrid(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    gridBarang->CreateGrid(0, 5);
    gridBarang->SetColLabelValue(0, "ID");
    gridBarang->SetColLabelValue(1, "Nama");
    gridBarang->SetColLabelValue(2, "Harga");
    gridBarang->SetColLabelValue(3, "Harga Beli");
    gridBarang->SetColLabelValue(4, "Jumlah");
    mainSizer->Add(gridBarang, 1, wxEXPAND | wxALL, 5);

    panel->SetSizer(mainSizer);
    LoadDataToGrid();  // Initial data load
}

BarangFrame::~BarangFrame() {
    sqlite3_close(db);
}

void BarangFrame::SetupDatabase() {
    const char* sql = "CREATE TABLE IF NOT EXISTS barang ("
                      "id TEXT PRIMARY KEY, "
                      "nama TEXT, "
                      "harga REAL, "
                      "harga_beli REAL, "
                      "jumlah INTEGER);";
    sqlite3_exec(db, sql, 0, 0, 0);
}

void BarangFrame::LoadDataToGrid(const wxString& searchQuery) {
    gridBarang->ClearGrid();
    while (gridBarang->GetNumberRows() > 0) {
        gridBarang->DeleteRows(0);
    }

	sqlite3_stmt* stmt;
	wxString sql = "SELECT id, nama, harga, harga_beli, jumlah FROM barang";
	if (!searchQuery.IsEmpty()) {
	    sql += " WHERE id LIKE '%" + searchQuery + "%' OR nama LIKE '%" + searchQuery + "%'";
	}
	sql += ";";


    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int row = gridBarang->GetNumberRows();
            gridBarang->AppendRows(1);
            gridBarang->SetCellValue(row, 0, wxString::FromUTF8((const char*)sqlite3_column_text(stmt, 0)));
            gridBarang->SetCellValue(row, 1, wxString::FromUTF8((const char*)sqlite3_column_text(stmt, 1)));
            gridBarang->SetCellValue(row, 2, wxString::Format("%.2f", sqlite3_column_double(stmt, 2)));
            gridBarang->SetCellValue(row, 3, wxString::Format("%.2f", sqlite3_column_double(stmt, 3)));
            gridBarang->SetCellValue(row, 4, wxString::Format("%d", sqlite3_column_int(stmt, 4)));
        }
    }
    sqlite3_finalize(stmt);
}

bool BarangFrame::SimpanBarang(const wxString& id, const wxString& nama, double harga, double hargaBeli, int jumlah) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO barang (id, nama, harga, harga_beli, jumlah) VALUES (?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, nama.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 3, harga);
    sqlite3_bind_double(stmt, 4, hargaBeli);
    sqlite3_bind_int(stmt, 5, jumlah);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    LoadDataToGrid();
    return success;
}

bool BarangFrame::EditBarang(const wxString& id, const wxString& nama, double harga, double hargaBeli, int jumlah) {
    sqlite3_stmt* stmt;
    const char* sql = "UPDATE barang SET nama = ?, harga = ?, harga_beli = ?, jumlah = ? WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, nama.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 2, harga);
    sqlite3_bind_double(stmt, 3, hargaBeli);
    sqlite3_bind_int(stmt, 4, jumlah);
    sqlite3_bind_text(stmt, 5, id.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    LoadDataToGrid();
    return success;
}

bool BarangFrame::HapusBarang(const wxString& id) {
    sqlite3_stmt* stmt;
    const char* sql = "DELETE FROM barang WHERE id = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    LoadDataToGrid();
    return success;
}

void BarangFrame::OnSimpanBarang(wxCommandEvent& event) {
    wxString id = txtID->GetValue();
    wxString nama = txtNama->GetValue();
    double harga = wxAtof(txtHarga->GetValue());
    double hargaBeli = wxAtof(txtHargaBeli->GetValue());
    long jumlah;
    txtJumlah->GetValue().ToLong(&jumlah);

    if (SimpanBarang(id, nama, harga, hargaBeli, jumlah)) {
        wxMessageBox("Barang berhasil disimpan.");
    } else {
        wxMessageBox("Gagal menyimpan barang.");
    }
}

void BarangFrame::OnEditBarang(wxCommandEvent& event) {
    wxString id = txtID->GetValue();
    wxString nama = txtNama->GetValue();
    double harga = wxAtof(txtHarga->GetValue());
    double hargaBeli = wxAtof(txtHargaBeli->GetValue());
    long jumlah;
    txtJumlah->GetValue().ToLong(&jumlah);

    if (EditBarang(id, nama, harga, hargaBeli, jumlah)) {
        wxMessageBox("Barang berhasil diedit.");
    } else {
        wxMessageBox("Gagal mengedit barang.");
    }
}

void BarangFrame::OnHapusBarang(wxCommandEvent& event) {
    wxString id = txtID->GetValue();

    if (HapusBarang(id)) {
        wxMessageBox("Barang berhasil dihapus.");
    } else {
        wxMessageBox("Gagal menghapus barang.");
    }
}

void BarangFrame::OnGridClick(wxGridEvent& event) {
    int row = event.GetRow();

    if (row >= 0 && row < gridBarang->GetNumberRows()) {
        txtID->SetValue(gridBarang->GetCellValue(row, 0));
        txtNama->SetValue(gridBarang->GetCellValue(row, 1));
        txtHarga->SetValue(gridBarang->GetCellValue(row, 2));
        txtHargaBeli->SetValue(gridBarang->GetCellValue(row, 3));
        txtJumlah->SetValue(gridBarang->GetCellValue(row, 4));
    }
}

void BarangFrame::OnSearch(wxCommandEvent& event) {
    LoadDataToGrid(txtSearch->GetValue());
}

class MyApp : public wxApp {
public:
    virtual bool OnInit() {
        BarangFrame* frame = new BarangFrame("Aplikasi CRUD Barang");
        frame->Show(true);
        frame->Maximize();
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);

