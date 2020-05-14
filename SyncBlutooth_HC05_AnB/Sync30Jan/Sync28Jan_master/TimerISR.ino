
void timer_ISR()
{
  sync_counter++;
  if(sync_counter>=t_limit)
  {
    sync_counter = 0;
    if(synced==true)
    {
      DoInSync();
    }
  }
}


