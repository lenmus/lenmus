//#ifdef __GNUG__
//// #pragma interface
//#endif
//
//#ifndef __THEOINTERVALS_H__		//to avoid nested includes
//#define __THEOINTERVALS_H__
//
//// For compilers that support precompilation, includes "wx/wx.h".
//#include "wx/wxprec.h"
//
//#ifdef __BORLANDC__
//#pragma hdrstop
//#endif
//
//#ifndef WX_PRECOMP
//#include "wx/wx.h"
//#endif
//
//
//class TheoIntervals : public wxWindow	
//{
//   DECLARE_DYNAMIC_CLASS(TheoIntervals)
//
//public:
//
//	// constructor and destructor    
//	TheoIntervals(wxWindow* parent, wxWindowID id, 
//			   const wxPoint& pos = wxDefaultPosition, 
//			   const wxSize& size = wxDefaultSize, int style = 0);
//
//	~TheoIntervals();
//
//	// event handlers
//	void OnOK(wxCommandEvent& event);
//	void OnSize(wxSizeEvent& event);
//
//private:
//
//
//    DECLARE_EVENT_TABLE()
//};
//
//
//
//#endif  // __THEOINTERVALS_H__
//
////Ejercicios de teoría sobre intervalos. Funciones para ser llamadas desde páginas xml u otros
////formularios
//
//	CfgTheoIntervals*	m_pConfig;		//opciones de configuración
//	bool				m_fConocidoIntervalo;    //true:  modo conocido intervalo
//	wxString			m_sSolucion;
//	int	m_ntN[2];		// the two notes
//	Score*	m_oPart;
//	int	m_ntMidi[2];
//
//	bool	m_fRespEnabled;       //Hay problema. Por tanto, se puede mostrar su solución
//	bool	m_fTocarEnabled;      //Ejecución del problema/solución habilitada
////Private m_picX As PictureBox            //donde se dibujan problema y solución
//
////colores para los enlaces de las respuestas
//	int colorVerde As Long, colorRojo As Long, colorAzul As Long, colorNormal As Long
//
//////contadores de respuestas correctas/fallidas
//	int			m_nCorrectas;
//	int			m_nFallidas;
//	wxString	m_lblAcertadas As Label
//	wxString	m_lblFalladas As Label
//	wxString	m_lblNota As Label
////
////
//
//	
//TheoIntervals::TheoIntervals(PictureBox picX, CfgTheoIntervals* pAjustes)
//{
////    Set m_picX = picX
////    m_picX.Cls
//    m_fRespEnabled = false;
//    m_fTocarEnabled = false;
//    
//    m_pConfig = pAjustes
//    
//    //initialize colors
//	//TODO: user selectable in 'preferences' dialog
//    colorVerde = &H1FF00;
//    colorRojo = &H8080FF;
//    colorAzul = &HFF0000;
//    colorNormal = &HFFE0E0;
//    
//}
//
//
////Connect other controls to this one
//void TheoIntervals::SetCounters(lblAcertadas As Label, lblFalladas As Label, lblNota As Label)
//    Set m_lblAcertadas = lblAcertadas
//    Set m_lblFalladas = lblFalladas
//    Set m_lblNota = lblNota
//    m_lblAcertadas.Caption = "0"
//    m_lblFalladas.Caption = "0"
//    m_lblNota.Caption = wxEmptyString
//
//}
//
////Connect button controls to this object
//void TheoIntervals::SetBoton(iB As Long, wxButton* pButton)
//{
//    wxASSERT(iB >= 0 && iB < 48);
//    
//    static wxString sEtq[48];
//    static bool fTablaCargada = false;
//    
//    if (!fTablaCargada) {
//        sEtq[0] = _("Unison");       //unisono y dobe disminuidas
//        sEtq[1] = _("2dd");
//        sEtq[3] = _("3dd");
//        sEtq[4] = _("4dd");
//        sEtq[5] = _("5dd");
//        sEtq[6] = _("6dd");
//        sEtq[7] = _("7dd");
//        sEtq[8] = _("8dd");
//        sEtq[9] = wxEmptyString;    //Disminuidas
//        sEtq[10] = _("2d");
//        sEtq[11] = _("3d");
//        sEtq[12] = _("4d");
//        sEtq[13] = _("5d");
//        sEtq[14] = _("6d");
//        sEtq[15] = _("7d");
//        sEtq[16] = _("8d");
//        sEtq[17] = wxEmptyString;   //menores
//        sEtq[18] = _("2m");
//        sEtq[19] = _("3m");
//        sEtq[20] = _("");
//        sEtq[21] = _("");
//        sEtq[22] = _("6m");
//        sEtq[23] = _("7m");
//        sEtq[24] = _("");
//        sEtq[25] = wxEmptyString;   //mayores y justas
//        sEtq[26] = _("2M");
//        sEtq[27] = _("3M");
//        sEtq[28] = _("4j");
//        sEtq[29] = _("5j");
//        sEtq[30] = _("6M");
//        sEtq[31] = _("7M");
//        sEtq[32] = _("8j");
//        sEtq[33] = wxEmpty;String   //aumentadas
//        sEtq[34] = _("2a");
//        sEtq[35] = _("3a");
//        sEtq[36] = _("4a");
//        sEtq[37] = _("5a");
//        sEtq[38] = _("6a");
//        sEtq[39] = _("7a");
//        sEtq[40] = _("8a");
//        sEtq[41] = wxEmpty;String   //doble aumentadas
//        sEtq[42] = _("2da");
//        sEtq[43] = _("3da");
//        sEtq[44] = _("4da");
//        sEtq[45] = _("5da");
//        sEtq[46] = _("6da");
//        sEtq[47] = _("7da");
//        sEtq[48] = _("8da");
//        fTablaCargada = true;
//    }
//    
//    m_pAnswerButton[iB] = pButton;
//    pButton->SetLabel(sEtq[iB]);
//    pButton->Visible = !sEtq[iB].IsEmpty();
//    
//}
//
//////Ordenes
////// Para simplificar el análisis los componentes van separados por "/"
//////   NewProblem
//////   TocarProblema
//////   MostrarSolucion
////
////void TheoIntervals::EjecutarOrden(ByVal sOrden As String) As String
////    Dim i As Long, sCmd As String, nDato1 As Long
////    
////    sCmd = sOrden
////    i = InStr(sOrden, "/")
////    if (i <> 0) {
////        sCmd = Left$(sOrden, i - 1)
////        nDato1 = CLng(Mid$(sOrden, i + 1))
////    }
////    
////    EjecutarOrden = wxEmptyString
////    Select case sCmd
////        case "NewProblem"
////            NewProblem
////        case "TocarProblema"
////            TocarProblema
////        case "MostrarSolucion"
////            MostrarSolucion
////        case "Respuesta"
////            Respuesta nDato1
////        case "DumpPartitura"
////            EjecutarOrden = DumpPartitura
////        case "FuentePartitura"
////            EjecutarOrden = FuentePartitura
////        case "ResetCounters"
////            ResetCounters
////        case "SetTipoProblema"
////            m_pConfig->nTipoProblema = nDato1
////        default:
////            Debug.Assert false
////    }
////
////}
//
//void TheoIntervals::NewProblem()
//{
//	int nContexto[7];
//	Note* oNota[2];
//	EClaves nClave;
//
//    ResetExercise();
//
//	// choose type of problem
//	switch (m_pConfig->nTipoProblema)
//	{
//		case etpFTI_IdentificarIntervalo:
//            m_fConocidoIntervalo = true;
//			break;
//		case etpFTI_CalcularIntervalo:
//            m_fConocidoIntervalo = false;
//			break;
//		case etpFTI_Ambos:
//            m_fConocidoIntervalo = (Rnd() > 0.5);
//			break;
//    }
//    
//    
//    //Generate two random notes satisfying problem constrains
//	RandomGenerator oGenerator();
//	for (int i = 0; i < 7; i++) {
//        nContexto[i] = 0;
//	}
//    nClave = oGenerator.GenerateClef();
//    m_ntN[0] = AltDiatonicaAleatoria(0, 8, false, nClave);
//    m_ntN[1] = AltDiatonicaAleatoria(0, 8, false, nClave);
//	while (m_ntN[0] == m_ntN[1]) {
//        m_ntN[1] = AltDiatonicaAleatoria(0, 8, false, nClave);
//	}
//    
//	//Convert problem to LDP music pattern
//    wxString sPatron[2], sAlter[2];
//	for (int i=0; i < 2; i++) {
//        sPatron[i] = _T("(N ");
//        sAlter[i] = wxEmptyString;
//        if (m_pConfig->fAlteraciones && (Rnd() > 0.5)) {
//			sAlter[i] = (Rnd() > 0.5 ? _T("-") : _T("+"));
//            sPatron[i] = sPatron[i] + sAlter[i];
//        }
//        sPatron[i] = sPatron[i] + GetNombreSajon(m_ntN[i]) + _T(" R)");
//	}
//    
//    //create the score
//    Score* m_oPart = new Score();
//    VStaff* m_pVStaff = m_oPart->Pentagrama(0);
//    InicializarParseLinea               //Para que CrearFigsil funcione correctamente
//    m_pVStaff->AddClef( nClave );
//    m_pVStaff->AddKeySignature( earmDo );
//    m_pVStaff->AddTimeSignature( emtr44, NO_VISIBLE );
//    m_pVStaff->AddEspacio 24
//    oNota[0] = m_pVStaff->CrearFigsil(sPatron[0])     //crea la nota y guarda referencia a la nota creada
//    m_pVStaff->AddBarra etbBarraNormal, NO_VISIBLE        //para que no se propague la alteración de la 1ª nota
//    oNota[1] = m_pVStaff->CrearFigsil(sPatron[1])     //crea la nota y guarda referencia a la nota creada
//    m_pVStaff->AddBarra etbBarraFinal, NO_VISIBLE
//
//    
////    //determinar la solución
////    Dim oIntv As CIntervalo
////    Set oIntv = New CIntervalo
////    oIntv.AnalizarIntervalo oNota[0], oNota[1], earmDo
////    m_sSolucion = oIntv.Nombre & IIf(oIntv.EsAscendente, _(", ascendent"), _(", descendent"))
//////                                IIf(oIntv.EsAscendente, ", ascendente", ", descendente")
////    m_ntMidi[0] = oIntv.NotaMidi1
////    m_ntMidi[1] = oIntv.NotaMidi2
////    
////    //determina el índice del botón correspondiente a la respuesta correcta
////    Dim iBoton As Long
////    With oIntv
////        Select case .Tipo
////            case eti_DobleDisminuido
////                //fila 1
////                m_nRespIndex = .Intervalo
////            case eti_Disminuido
////                //fila 2
////                m_nRespIndex = .Intervalo + 8
////            case eti_Menor
////                //fila 3
////                m_nRespIndex = .Intervalo + 16
////            case eti_Mayor, eti_Justo
////                //fila 4
////                m_nRespIndex = .Intervalo + 24
////            case eti_Aumentado
////                //fila 5
////                m_nRespIndex = .Intervalo + 32
////            case eti_DobleAumentado
////                //fila 6
////                m_nRespIndex = .Intervalo + 40
////        }
////    End With
////    
////    
////    //presentar el problema
////    if (m_fConocidoIntervalo) {        //modo directo
////        PlantearProblema m_picX, m_oPart
////    } else {
////        m_sSolucion = m_sSolucion & sCrLf & "  empezando en " & GetNombreLng_Nota(m_ntN[0])
////        if (sAlter[0] <> wxEmptyString) { m_sSolucion = m_sSolucion & IIf(sAlter[0] = "+", " #", " b")
////        EscribirProblema m_picX, m_sSolucion
////    }
////    m_fTocarEnabled = false
////    m_fRespEnabled = true
////    
////    if (FMain.fFrmPiano) { FPiano.DesmarcarTeclas
//    
//}
//
////void TheoIntervals::TocarProblema()
////    TocarPartitura m_oPart, 320, SIN_METRONOMO, eTEJ_NormalInstrument, CON_SEGUIMIENTO
////}
////
////void TheoIntervals::MostrarSolucion()
////    
////    if (m_fConocidoIntervalo) {
////        PlantearProblema m_picX, m_oPart, , m_sSolucion
////    } else {
////        PlantearProblema m_picX, m_oPart, m_sSolucion
////    }
////    
////    //si el piano está visible marca las notas
////    if (FMain.fFrmPiano) {
////        With FPiano
////            .HabilitarMarcado = true
////            .MarcarTecla m_ntMidi[0]
////            .MarcarTecla m_ntMidi[1]
////            .HabilitarMarcado = false
////        End With
////    }
////
////    m_fTocarEnabled = true
////    m_fRespEnabled = false
////    
////}
////
////void TheoIntervals::Respuesta(Index As Long)
////{
////
////    if (!m_fButtonsEnabled) { return;
////    
////    //la botonera sólo se activa en problemas de tipo fConocidoIntervalo
////    Dim i As Long, fAcierto As Boolean
////    Dim nBrillo As Long
////    
////    //determinar si acierto o fallo
////    if (!m_fConocidoIntervalo) { return;
////    
////    fAcierto = (Index = m_nRespIndex)
////    
////    //pone sonido y colores, y actualiza contadores
////    if (fAcierto) {
////        nBrillo = colorVerde
////        SonidoAcierto
////        m_nCorrectas = m_nCorrectas + 1
////    } else {
////        nBrillo = colorRojo
////        SonidoFallo
////        m_nFallidas = m_nFallidas + 1
////    }
////        
////    //muestra las estadísticas
////    Dim rAciertos As Single
////    rAciertos = 10# * CSng(m_nCorrectas) / CSng(m_nCorrectas + m_nFallidas)
////    if (!m_lblAcertadas Is Nothing) { m_lblAcertadas.Caption = m_nCorrectas
////    if (!m_lblFalladas Is Nothing) { m_lblFalladas.Caption = m_nFallidas
////    if (!m_lblNota Is Nothing) {
////        if (rAciertos = 10#) {
////            m_lblNota.Caption = "10"
////        } else if { rAciertos = 0#) {
////            m_lblNota.Caption = "0"
////        } else {
////            m_lblNota.Caption = Format$(Round(rAciertos, 1), "#0.0")
////        }
////    }
////
////    //si fallo, muestra la solución. Si acierto, genera nuevo problema
////    if (!fAcierto) {
////        //dejar marcado el botón correcto en verde y el pulsado en rojo
////        if (TypeOf m_pAnswerButton[Index) Is Label) {
////            m_pAnswerButton[m_nRespIndex).ForeColor = colorVerde
////            m_pAnswerButton[Index).ForeColor = colorRojo
////        } else if { TypeOf m_pAnswerButton[Index) Is CommandButton) {
////            m_pAnswerButton[m_nRespIndex).BackColor = colorVerde
////            m_pAnswerButton[Index).BackColor = colorRojo
////        }
////        
////        //mostrar la solución
////        MostrarSolucion
////        HabilitarBotonera false
////    
////    } else {
////        NewProblem
////    }
////    
////}
////
////void TheoIntervals::DumpPartitura() As String
////    if (m_oPart Is Nothing) {
////        DumpPartitura = "Partitura vacia"
////    } else {
////        DumpPartitura = m_oPart.Dump
////    }
////}
////
////void TheoIntervals::FuentePartitura() As String
////    if (m_oPart Is Nothing) {
////        FuentePartitura = "Partitura vacia"
////    } else {
////        FuentePartitura = m_oPart.Fuente
////    }
////}
////
////void TheoIntervals::ResetExercise()
////    Dim i As Long
////    For i = 1 To 48
////        if (!m_cmdResp[i] Is Nothing) {
////            if (TypeOf m_cmdResp[i] Is Label) {
////                m_cmdResp[i].ForeColor = colorAzul
////            } else if { TypeOf m_cmdResp[i] Is CommandButton) {
////                m_cmdResp[i].BackColor = colorNormal
////            }
////        }
////    Next i
////    HabilitarBotonera true
////    
////}
////
////
//void TheoIntervals::ResetCounters()
//{
//    m_nCorrectas = 0;
//    m_nFallidas = 0;
//    
//    if (m_lblAcertadas != wxEmptyString) m_lblAcertadas = m_nCorrectas;
//    if (m_lblFalladas != wxEmptyString) m_lblFalladas = m_nFallidas;
//    if (m_lblNota != wxEmptyString) m_lblNota = wxEmptyString;
//    
//}
//
///*
//*/