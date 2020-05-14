

void timer_ISR()
{
 sync_counter++;
 if(sync_counter>=t_limit)
 {
   t_limit = 0;
   if(synced==true)
   {
     DoInSync();
   }
   else
   {
   }
 }

 
 
 
}

