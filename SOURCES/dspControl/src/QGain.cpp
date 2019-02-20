#include <QSlider>
#include <QDebug>

#include "QVolumeSlider.hpp"

#include "QGain.hpp"
#include "ui_QGain.h"

using namespace Vektorraum;

extern QVolumeSlider* sliderMainVolume;

QGain::QGain( tfloat V0, uint16_t gainaddr, CFreeDspAurora* ptrdsp, QWidget *parent ) :
  QDspBlock(parent), ui(new Ui::QGain)
{
  addr[kTargetGain] = gainaddr;

  dsp = ptrdsp;

  ui->setupUi(this);
  ui->doubleSpinBoxGain->blockSignals( true );
  ui->doubleSpinBoxGain->setValue( V0 );
  ui->doubleSpinBoxGain->setAttribute( Qt::WA_MacShowFocusRect, 0 );
  ui->doubleSpinBoxGain->blockSignals( false );
}

QGain::~QGain()
{
  delete ui;
}

//------------------------------------------------------------------------------
/*! \brief Updates the filter.
 *
 */
void QGain::update( tvector<tfloat> f )
{
  H = tvector<tcomplex>( length(f) );
  if( bypass )
  {
    for( tuint ii = 0; ii < length(f); ii++ )
      H[ii] = 1.0;
  }
  else
  {
    for( tuint ii = 0; ii < length(f); ii++ )
      H[ii] = pow( 10.0, ui->doubleSpinBoxGain->value()/20.0 );
  }
}

//------------------------------------------------------------------------------
/*!
 *
 */
void QGain::on_doubleSpinBoxGain_valueChanged( double )
{
  emit valueChanged();
  sendDspParameter();
}

//------------------------------------------------------------------------------
/*!
 *
 */
void QGain::on_pushButtonBypass_clicked()
{
  bypass = ui->pushButtonBypass->isChecked();
  sendDspParameter();
  emit valueChanged();
}

//------------------------------------------------------------------------------
/*!
 *
 */
void QGain::sendDspParameter( void )
{
  double gain = static_cast<double>(sliderMainVolume->value());
  if( !bypass )
    gain += ui->doubleSpinBoxGain->value();
  qDebug()<<sliderMainVolume->value()<<ui->doubleSpinBoxGain->value()<<gain;
  float val = static_cast<float>(pow( 10.0, gain/20.0 ));
  dsp->sendParameter( addr[kTargetGain], val );
}

//------------------------------------------------------------------------------
/*!
 *
 */
uint32_t QGain::getNumBytes( void )
{
  return 6;
}

//------------------------------------------------------------------------------
/*!
 *
 */
void QGain::writeDspParameter( void )
{
  float val = static_cast<float>(pow( 10.0, ui->doubleSpinBoxGain->value()/20.0 ));
  if( bypass )
    val = 1.0;
  dsp->storeRegAddr( addr[kTargetGain] );
  dsp->storeValue( val );
}